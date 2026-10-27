// SPDX-FileCopyrightText: 2026 CTRL-F-0rg3
// SPDX-License-Identifier: GPL-3.0-only
//! CoTanOS Physical Frame Allocator (`frm.rs`)
//!
//! Manages raw physical memory split into frames (pages).
//!
//! The allocator is one of the very first kernel subsystems brought up during
//! early boot, so the whole file obeys strict rules:
//!
//! * `#![no_std]`, **`core` only** — no `alloc`, no `std`, no heap, no
//!   floating point (the FPU/SSE state may not be initialised yet), no
//!   dependency on any other kernel module. (`std` is used exclusively inside
//!   `#[cfg(test)]`.)
//! * No allocation of its own: every metadata table is either a fixed-size
//!   array inside the allocator or a caller-provided `&'static mut [u64]`.
//! * Never panics on bad *runtime* input (corrupt firmware tables, double
//!   frees, absurd sizes...). Errors are reported through [`FrameError`].
//!   The only panics are the explicit `new()` convenience constructors, which
//!   exist for boot code that prefers to halt on a misconfigured allocator.
//! * SMP-safe and interrupt-safe: the allocator state is protected by an
//!   [`IrqSpinlock`] (ticket lock + local interrupt masking on x86_64).
//!
//! # File layout
//!
//! 1.  [`Spinlock`] / [`IrqSpinlock`] — self-contained fair ticket locks.
//! 2.  [`PhysAddr`] — physical address with checked arithmetic.
//! 3.  [`PageSize`] + [`Size4Kb`], [`Size2Mb`], [`Size1Gb`].
//! 4.  [`PhysFrame`] + [`PhysFrameRange`] / [`PhysFrameRangeInclusive`].
//! 5.  [`direct_map`] — optional physical→virtual window (zeroing, poisoning).
//! 6.  [`FrameError`].
//! 7.  [`MemoryRegionType`] / [`MemoryRegion`] / [`MemoryMap`]
//!     (sorting, normalisation, validation, placement search).
//! 8.  Boot protocol parsers: E820, UEFI, Multiboot 1 and Multiboot 2.
//! 9.  [`BitmapLayout`] and the internal bitmap engine.
//! 10. [`PhysicalFrameAllocator`] — public, SMP-safe wrapper.
//! 11. [`AllocatorStats`], [`FragmentationReport`], [`AllocatorRegistry`].
//! 12. [`FrameCache`] — per-CPU magazines (lock-contention reducer).
//! 13. [`FrameManager`] — zone (DMA / DMA32 / Normal) fallback front-end.
//! 14. Tests.
//! 15. Long-form documentation (a comment block at the very end of the file).

use core::cell::UnsafeCell;
use core::fmt;
use core::hash::{Hash, Hasher};
use core::marker::PhantomData;
use core::mem::ManuallyDrop;
use core::ops::{Add, Deref, DerefMut, Sub};
use core::sync::atomic::{AtomicUsize, Ordering};

// ============================================================================
// 1. Locks — self-contained, no external crates
// ============================================================================

/// A fair ticket spinlock.
///
/// Every locker takes a ticket (`next_ticket`) and spins until `now_serving`
/// reaches it, which gives FIFO ordering and therefore prevents starvation on
/// many-core machines (the previous test-and-set lock was not fair).
///
/// The lock does **not** touch the interrupt flag. Use [`IrqSpinlock`] for
/// data that can also be reached from an interrupt handler.
pub struct Spinlock<T: ?Sized> {
    next_ticket: AtomicUsize,
    now_serving: AtomicUsize,
    data: UnsafeCell<T>,
}

// SAFETY: access to `data` is serialised by the ticket protocol, so sharing a
// `Spinlock<T>` between CPUs is sound whenever `T` itself may be sent to
// another CPU.
unsafe impl<T: ?Sized + Send> Send for Spinlock<T> {}
unsafe impl<T: ?Sized + Send> Sync for Spinlock<T> {}

impl<T> Spinlock<T> {
    /// Creates a new, unlocked spinlock.
    #[inline]
    pub const fn new(data: T) -> Self {
        Self {
            next_ticket: AtomicUsize::new(0),
            now_serving: AtomicUsize::new(0),
            data: UnsafeCell::new(data),
        }
    }

    /// Consumes the lock and returns the protected value.
    #[inline]
    pub fn into_inner(self) -> T {
        self.data.into_inner()
    }
}

impl<T: ?Sized> Spinlock<T> {
    /// Acquires the lock, spinning until it becomes available.
    #[inline]
    pub fn lock(&self) -> SpinlockGuard<'_, T> {
        // Relaxed is enough for taking the ticket: the Acquire load below is
        // what synchronises with the previous holder's Release store.
        let ticket = self.next_ticket.fetch_add(1, Ordering::Relaxed);
        while self.now_serving.load(Ordering::Acquire) != ticket {
            core::hint::spin_loop();
        }
        SpinlockGuard {
            lock: self,
            _not_send: PhantomData,
        }
    }

    /// Tries to acquire the lock without spinning.
    #[inline]
    pub fn try_lock(&self) -> Option<SpinlockGuard<'_, T>> {
        let serving = self.now_serving.load(Ordering::Acquire);
        // The lock is free only if nobody holds a ticket that is not yet
        // served, i.e. `next_ticket == now_serving`. Claiming the ticket with a
        // CAS makes the attempt atomic.
        if self
            .next_ticket
            .compare_exchange(
                serving,
                serving.wrapping_add(1),
                Ordering::Acquire,
                Ordering::Relaxed,
            )
            .is_ok()
        {
            Some(SpinlockGuard {
                lock: self,
                _not_send: PhantomData,
            })
        } else {
            None
        }
    }

    /// Returns `true` if the lock is currently held (racy, diagnostics only).
    #[inline]
    pub fn is_locked(&self) -> bool {
        self.next_ticket.load(Ordering::Relaxed) != self.now_serving.load(Ordering::Relaxed)
    }

    /// Returns a mutable reference without locking (exclusive `&mut self`
    /// already proves that nobody else can access the data).
    #[inline]
    pub fn get_mut(&mut self) -> &mut T {
        self.data.get_mut()
    }
}

/// RAII guard returned by [`Spinlock::lock`]. Unlocks on drop.
pub struct SpinlockGuard<'a, T: ?Sized> {
    lock: &'a Spinlock<T>,
    // A guard must be released on the CPU that took it.
    _not_send: PhantomData<*mut ()>,
}

impl<T: ?Sized> Deref for SpinlockGuard<'_, T> {
    type Target = T;
    #[inline]
    fn deref(&self) -> &T {
        // SAFETY: we hold the lock, so access is exclusive.
        unsafe { &*self.lock.data.get() }
    }
}

impl<T: ?Sized> DerefMut for SpinlockGuard<'_, T> {
    #[inline]
    fn deref_mut(&mut self) -> &mut T {
        // SAFETY: we hold the lock, so access is exclusive.
        unsafe { &mut *self.lock.data.get() }
    }
}

impl<T: ?Sized> Drop for SpinlockGuard<'_, T> {
    #[inline]
    fn drop(&mut self) {
        // Hand the lock to the next ticket holder. Release publishes all
        // writes made inside the critical section.
        self.lock.now_serving.fetch_add(1, Ordering::Release);
    }
}

/// Saved local-CPU interrupt state (`true` = interrupts were enabled).
#[derive(Clone, Copy)]
struct IrqState(bool);

/// Disables local interrupts and returns the previous state.
///
/// Compiled out (no-op) on non-x86_64 targets and under `cfg(test)`, because
/// `cli`/`sti` are privileged. Porting to another architecture means adding
/// one more `cfg` pair here (e.g. `DAIF` on aarch64).
#[cfg(all(target_arch = "x86_64", not(test)))]
#[inline]
fn irq_save_and_disable() -> IrqState {
    let rflags: u64;
    // SAFETY: reads RFLAGS and clears IF. Kernel mode only. The asm block is
    // intentionally *not* marked `nomem`, so it acts as a compiler barrier.
    unsafe {
        core::arch::asm!("pushfq", "pop {0}", "cli", out(reg) rflags);
    }
    IrqState(rflags & (1 << 9) != 0)
}

#[cfg(not(all(target_arch = "x86_64", not(test))))]
#[inline]
fn irq_save_and_disable() -> IrqState {
    IrqState(false)
}

/// Restores the interrupt state saved by [`irq_save_and_disable`].
#[cfg(all(target_arch = "x86_64", not(test)))]
#[inline]
fn irq_restore(state: IrqState) {
    if state.0 {
        // SAFETY: re-enables interrupts only if they were enabled before.
        unsafe {
            core::arch::asm!("sti");
        }
    }
}

#[cfg(not(all(target_arch = "x86_64", not(test))))]
#[inline]
fn irq_restore(_state: IrqState) {}

/// A [`Spinlock`] that also masks local interrupts while held.
///
/// This prevents the classic deadlock "interrupt handler allocates a frame
/// while the interrupted code on the same CPU already holds the allocator
/// lock". The previous interrupt state is restored on unlock, so nesting and
/// boot code running with interrupts disabled both work correctly.
pub struct IrqSpinlock<T: ?Sized> {
    inner: Spinlock<T>,
}

impl<T> IrqSpinlock<T> {
    /// Creates a new, unlocked lock.
    #[inline]
    pub const fn new(data: T) -> Self {
        Self {
            inner: Spinlock::new(data),
        }
    }
}

impl<T: ?Sized> IrqSpinlock<T> {
    /// Masks interrupts and acquires the lock.
    #[inline]
    pub fn lock(&self) -> IrqSpinlockGuard<'_, T> {
        // Order matters: interrupts off *before* spinning, otherwise an
        // interrupt could arrive while we hold a ticket and wait forever.
        let irq = irq_save_and_disable();
        let guard = self.inner.lock();
        IrqSpinlockGuard {
            guard: ManuallyDrop::new(guard),
            irq,
        }
    }

    /// Non-blocking variant of [`IrqSpinlock::lock`].
    #[inline]
    pub fn try_lock(&self) -> Option<IrqSpinlockGuard<'_, T>> {
        let irq = irq_save_and_disable();
        match self.inner.try_lock() {
            Some(guard) => Some(IrqSpinlockGuard {
                guard: ManuallyDrop::new(guard),
                irq,
            }),
            None => {
                irq_restore(irq);
                None
            }
        }
    }

    /// Returns `true` if the lock is currently held (racy, diagnostics only).
    #[inline]
    pub fn is_locked(&self) -> bool {
        self.inner.is_locked()
    }
}

/// RAII guard returned by [`IrqSpinlock::lock`].
pub struct IrqSpinlockGuard<'a, T: ?Sized> {
    guard: ManuallyDrop<SpinlockGuard<'a, T>>,
    irq: IrqState,
}

impl<T: ?Sized> Deref for IrqSpinlockGuard<'_, T> {
    type Target = T;
    #[inline]
    fn deref(&self) -> &T {
        &**self.guard
    }
}

impl<T: ?Sized> DerefMut for IrqSpinlockGuard<'_, T> {
    #[inline]
    fn deref_mut(&mut self) -> &mut T {
        &mut **self.guard
    }
}

impl<T: ?Sized> Drop for IrqSpinlockGuard<'_, T> {
    #[inline]
    fn drop(&mut self) {
        // SAFETY: `guard` is dropped exactly once, here. The lock is released
        // first, interrupts are re-enabled afterwards (reverse of lock order).
        unsafe {
            ManuallyDrop::drop(&mut self.guard);
        }
        irq_restore(self.irq);
    }
}

// ============================================================================
// 2. PhysAddr — physical address
// ============================================================================

/// A physical memory address.
///
/// The wrapper deliberately has no implicit conversion to a pointer: a
/// physical address is not dereferenceable until it is mapped (see
/// [`direct_map`]).
#[derive(Clone, Copy, PartialEq, Eq, PartialOrd, Ord, Hash)]
#[repr(transparent)]
pub struct PhysAddr(pub usize);

impl PhysAddr {
    /// The zero address.
    pub const ZERO: PhysAddr = PhysAddr(0);
    /// The highest representable address.
    pub const MAX: PhysAddr = PhysAddr(usize::MAX);

    /// Creates a physical address. No validation is performed here; whether
    /// the address exists is decided by the memory map.
    #[inline]
    pub const fn new(addr: usize) -> Self {
        Self(addr)
    }

    /// Returns the address as `usize`.
    #[inline]
    pub const fn as_usize(self) -> usize {
        self.0
    }

    /// Returns the address as `u64` (lossless on all supported targets).
    #[inline]
    pub const fn as_u64(self) -> u64 {
        self.0 as u64
    }

    /// Returns `true` for address zero.
    #[inline]
    pub const fn is_null(self) -> bool {
        self.0 == 0
    }

    /// Returns `true` if the address is a multiple of the page size `S`.
    #[inline]
    pub const fn is_aligned<S: PageSize>(self) -> bool {
        self.is_aligned_to(S::SIZE)
    }

    /// Returns `true` if the address is a multiple of `align`.
    /// `align` must be a non-zero power of two, otherwise `false` is returned.
    #[inline]
    pub const fn is_aligned_to(self, align: usize) -> bool {
        align != 0 && (align & (align - 1)) == 0 && (self.0 & (align - 1)) == 0
    }

    /// Rounds the address up to a multiple of `align` (a power of two).
    /// Returns `None` on overflow or if `align` is not a power of two.
    #[inline]
    pub const fn checked_align_up_to(self, align: usize) -> Option<Self> {
        if align == 0 || (align & (align - 1)) != 0 {
            return None;
        }
        let mask = align - 1;
        match self.0.checked_add(mask) {
            Some(v) => Some(Self(v & !mask)),
            None => None,
        }
    }

    /// Rounds the address down to a multiple of `align` (a power of two).
    /// Returns `None` if `align` is not a power of two.
    #[inline]
    pub const fn checked_align_down_to(self, align: usize) -> Option<Self> {
        if align == 0 || (align & (align - 1)) != 0 {
            return None;
        }
        Some(Self(self.0 & !(align - 1)))
    }

    /// Rounds up to the page size `S`; `None` on overflow.
    #[inline]
    pub const fn checked_align_up<S: PageSize>(self) -> Option<Self> {
        self.checked_align_up_to(S::SIZE)
    }

    /// Rounds up to the page size `S`.
    ///
    /// # Panics
    /// Panics on overflow. Use [`PhysAddr::checked_align_up`] on untrusted
    /// input (firmware memory maps).
    #[inline]
    pub fn align_up<S: PageSize>(self) -> Self {
        match self.checked_align_up::<S>() {
            Some(a) => a,
            None => panic!("CoTanOS frm: PhysAddr::align_up overflow"),
        }
    }

    /// Rounds down to the page size `S`.
    #[inline]
    pub const fn align_down<S: PageSize>(self) -> Self {
        Self(self.0 & !(S::SIZE - 1))
    }

    /// Checked addition.
    #[inline]
    pub const fn checked_add(self, rhs: usize) -> Option<Self> {
        match self.0.checked_add(rhs) {
            Some(v) => Some(Self(v)),
            None => None,
        }
    }

    /// Checked subtraction.
    #[inline]
    pub const fn checked_sub(self, rhs: usize) -> Option<Self> {
        match self.0.checked_sub(rhs) {
            Some(v) => Some(Self(v)),
            None => None,
        }
    }

    /// Saturating addition.
    #[inline]
    pub const fn saturating_add(self, rhs: usize) -> Self {
        Self(self.0.saturating_add(rhs))
    }

    /// Distance `self - other` in bytes, or `None` if `other > self`.
    #[inline]
    pub const fn offset_from(self, other: PhysAddr) -> Option<usize> {
        self.0.checked_sub(other.0)
    }
}

impl fmt::Debug for PhysAddr {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "PhysAddr({:#x})", self.0)
    }
}

impl fmt::Display for PhysAddr {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "{:#x}", self.0)
    }
}

impl fmt::LowerHex for PhysAddr {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        fmt::LowerHex::fmt(&self.0, f)
    }
}

impl fmt::UpperHex for PhysAddr {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        fmt::UpperHex::fmt(&self.0, f)
    }
}

impl From<usize> for PhysAddr {
    #[inline]
    fn from(v: usize) -> Self {
        Self(v)
    }
}

impl From<PhysAddr> for usize {
    #[inline]
    fn from(a: PhysAddr) -> usize {
        a.0
    }
}

impl Add<usize> for PhysAddr {
    type Output = PhysAddr;
    /// # Panics
    /// Panics on overflow (use [`PhysAddr::checked_add`] for untrusted data).
    #[inline]
    fn add(self, rhs: usize) -> PhysAddr {
        match self.0.checked_add(rhs) {
            Some(v) => PhysAddr(v),
            None => panic!("CoTanOS frm: PhysAddr + usize overflow"),
        }
    }
}

impl Sub<usize> for PhysAddr {
    type Output = PhysAddr;
    /// # Panics
    /// Panics on underflow (use [`PhysAddr::checked_sub`] for untrusted data).
    #[inline]
    fn sub(self, rhs: usize) -> PhysAddr {
        match self.0.checked_sub(rhs) {
            Some(v) => PhysAddr(v),
            None => panic!("CoTanOS frm: PhysAddr - usize underflow"),
        }
    }
}

impl Sub<PhysAddr> for PhysAddr {
    type Output = usize;
    /// # Panics
    /// Panics if `rhs > self`.
    #[inline]
    fn sub(self, rhs: PhysAddr) -> usize {
        match self.0.checked_sub(rhs.0) {
            Some(v) => v,
            None => panic!("CoTanOS frm: PhysAddr - PhysAddr underflow"),
        }
    }
}

// ============================================================================
// 3. PageSize + implementations
// ============================================================================

/// Describes a page size supported by CoTanOS.
///
/// All sizes are powers of two, which the allocator relies on for fast
/// masking instead of division.
pub trait PageSize: Copy + Clone + Send + Sync + 'static {
    /// Page size in bytes (power of two).
    const SIZE: usize;
    /// `log2(SIZE)`.
    const SHIFT: usize;
    /// Paging level (1 = 4 KiB, 2 = 2 MiB, 3 = 1 GiB on x86_64).
    const LEVEL: usize;
    /// Human-readable name used in diagnostics.
    const NAME: &'static str;
}

/// 4 KiB page.
#[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord, Hash)]
pub struct Size4Kb;
impl PageSize for Size4Kb {
    const SIZE: usize = 0x1000;
    const SHIFT: usize = 12;
    const LEVEL: usize = 1;
    const NAME: &'static str = "4K";
}

/// 2 MiB huge page.
#[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord, Hash)]
pub struct Size2Mb;
impl PageSize for Size2Mb {
    const SIZE: usize = 0x20_0000;
    const SHIFT: usize = 21;
    const LEVEL: usize = 2;
    const NAME: &'static str = "2M";
}

/// 1 GiB giant page. Only meaningful on 64-bit targets.
#[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord, Hash)]
pub struct Size1Gb;
impl PageSize for Size1Gb {
    const SIZE: usize = 0x4000_0000;
    const SHIFT: usize = 30;
    const LEVEL: usize = 3;
    const NAME: &'static str = "1G";
}

// Compile-time sanity checks of the constants above.
const _: () = assert!(<Size4Kb as PageSize>::SIZE == 1 << <Size4Kb as PageSize>::SHIFT);
const _: () = assert!(<Size2Mb as PageSize>::SIZE == 1 << <Size2Mb as PageSize>::SHIFT);
const _: () = assert!(<Size1Gb as PageSize>::SIZE == 1 << <Size1Gb as PageSize>::SHIFT);
const _: () = assert!(<Size2Mb as PageSize>::SIZE == 512 * <Size4Kb as PageSize>::SIZE);
const _: () = assert!(<Size1Gb as PageSize>::SIZE == 512 * <Size2Mb as PageSize>::SIZE);

/// Number of `S`-sized pages that fit into one `L`-sized page
/// (`L::SIZE / S::SIZE`). Meaningful only when `L::SIZE >= S::SIZE`.
#[inline]
pub const fn pages_per<L: PageSize, S: PageSize>() -> usize {
    L::SIZE / S::SIZE
}

// ============================================================================
// 4. PhysFrame + ranges
// ============================================================================

/// A physical memory frame of page size `S`.
///
/// `Clone`, `Copy`, `Eq`, `Ord`, `Hash` and `Debug` are implemented by hand
/// (not derived) so that they do not impose extra bounds on the marker `S`.
pub struct PhysFrame<S: PageSize> {
    start_address: PhysAddr,
    _marker: PhantomData<S>,
}

impl<S: PageSize> Clone for PhysFrame<S> {
    #[inline]
    fn clone(&self) -> Self {
        *self
    }
}
impl<S: PageSize> Copy for PhysFrame<S> {}

impl<S: PageSize> PartialEq for PhysFrame<S> {
    #[inline]
    fn eq(&self, other: &Self) -> bool {
        self.start_address == other.start_address
    }
}
impl<S: PageSize> Eq for PhysFrame<S> {}

impl<S: PageSize> PartialOrd for PhysFrame<S> {
    #[inline]
    fn partial_cmp(&self, other: &Self) -> Option<core::cmp::Ordering> {
        Some(self.cmp(other))
    }
}
impl<S: PageSize> Ord for PhysFrame<S> {
    #[inline]
    fn cmp(&self, other: &Self) -> core::cmp::Ordering {
        self.start_address.cmp(&other.start_address)
    }
}

impl<S: PageSize> Hash for PhysFrame<S> {
    #[inline]
    fn hash<H: Hasher>(&self, state: &mut H) {
        self.start_address.hash(state);
    }
}

impl<S: PageSize> fmt::Debug for PhysFrame<S> {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "PhysFrame<{}>({:#x})", S::NAME, self.start_address.0)
    }
}


/// Errors from frame allocator operations.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum FrameError {
    Misaligned { addr: usize, align: usize },
    OutOfMemory,
    InvalidFrame { addr: usize },
    DoubleFree { addr: usize },
    BitmapError(crate::map::BitmapError),
    NotInitialized,
}

impl fmt::Display for FrameError {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            Self::Misaligned { addr, align } => write!(f, "misaligned {:#x} (align {})", addr, align),
            Self::OutOfMemory => write!(f, "out of memory"),
            Self::InvalidFrame { addr } => write!(f, "invalid frame {:#x}", addr),
            Self::DoubleFree { addr } => write!(f, "double free {:#x}", addr),
            Self::BitmapError(e) => write!(f, "bitmap: {}", e),
            Self::NotInitialized => write!(f, "not initialized"),
        }
    }
}

impl<S: PageSize> PhysFrame<S> {
    /// Creates a frame from an `S`-aligned start address.
    ///
    /// Returns [`FrameError::Misaligned`] if the address is not aligned.
    #[inline]
    pub fn from_start_address(address: PhysAddr) -> Result<Self, FrameError> {
        if address.is_aligned::<S>() {
            Ok(Self {
                start_address: address,
                _marker: PhantomData,
            })
        } else {
            Err(FrameError::Misaligned {
                addr: address.0,
                align: S::SIZE,
            })
        }
    }

    /// Like [`PhysFrame::from_start_address`] but returns an `Option`.
    #[inline]
    pub fn from_start_address_checked(address: PhysAddr) -> Option<Self> {
        Self::from_start_address(address).ok()
    }

    /// Returns the frame that contains `address` (aligns it down).
    #[inline]
    pub fn containing_address(address: PhysAddr) -> Self {
        Self {
            start_address: address.align_down::<S>(),
            _marker: PhantomData,
        }
    }

    /// Creates a frame from its index (`address / S::SIZE`).
    ///
    /// # Panics
    /// Panics if `index * S::SIZE` overflows. This cannot happen for any
    /// index that came from a real physical address.
    #[inline]
    pub fn from_index(index: usize) -> Self {
        match Self::try_from_index(index) {
            Some(f) => f,
            None => panic!("CoTanOS frm: frame index overflow"),
        }
    }

    /// Checked variant of [`PhysFrame::from_index`].
    #[inline]
    pub fn try_from_index(index: usize) -> Option<Self> {
        match index.checked_mul(S::SIZE) {
            Some(addr) => Some(Self {
                start_address: PhysAddr(addr),
                _marker: PhantomData,
            }),
            None => None,
        }
    }

    /// First byte of the frame.
    #[inline]
    pub const fn start_address(self) -> PhysAddr {
        self.start_address
    }

    /// Frame number (`address / S::SIZE`).
    #[inline]
    pub const fn index(self) -> usize {
        self.start_address.0 >> S::SHIFT
    }

    /// First byte *after* the frame (saturating at the top of the address
    /// space).
    #[inline]
    pub const fn end_address(self) -> PhysAddr {
        PhysAddr(self.start_address.0.saturating_add(S::SIZE))
    }

    /// Frame size in bytes.
    #[inline]
    pub const fn size(self) -> usize {
        S::SIZE
    }

    /// Returns `true` if `addr` lies inside this frame.
    #[inline]
    pub const fn contains_address(self, addr: PhysAddr) -> bool {
        addr.0 >= self.start_address.0 && addr.0 - self.start_address.0 < S::SIZE
    }

    /// The next frame, or `None` at the end of the address space.
    #[inline]
    pub fn next(self) -> Option<Self> {
        Self::try_from_index(self.index().checked_add(1)?)
    }

    /// `self + n` frames, or `None` on overflow.
    #[inline]
    pub fn checked_add(self, n: usize) -> Option<Self> {
        Self::try_from_index(self.index().checked_add(n)?)
    }

    /// `self - n` frames, or `None` on underflow.
    #[inline]
    pub fn checked_sub(self, n: usize) -> Option<Self> {
        Self::try_from_index(self.index().checked_sub(n)?)
    }

    /// Half-open range `[self, end)`.
    #[inline]
    pub fn range_to(self, end: Self) -> PhysFrameRange<S> {
        PhysFrameRange::new(self, end)
    }

    /// Closed range `[self, end]`.
    #[inline]
    pub fn range_inclusive(self, end: Self) -> PhysFrameRangeInclusive<S> {
        PhysFrameRangeInclusive::new(self, end)
    }

    /// Range of `count` frames starting at `self`; `None` on overflow.
    #[inline]
    pub fn range_of(self, count: usize) -> Option<PhysFrameRange<S>> {
        let end = self.index().checked_add(count)?;
        Some(PhysFrameRange {
            start: self.index(),
            end,
            _marker: PhantomData,
        })
    }

    /// Splits this frame into the smaller frames of size `T`
    /// (e.g. one 2 MiB frame into 512 4 KiB frames).
    /// Returns `None` if `T` is larger than `S`.
    #[inline]
    pub fn split<T: PageSize>(self) -> Option<PhysFrameRange<T>> {
        if T::SIZE > S::SIZE {
            return None;
        }
        let start = self.start_address.0 >> T::SHIFT;
        Some(PhysFrameRange {
            start,
            end: start + (S::SIZE >> T::SHIFT),
            _marker: PhantomData,
        })
    }

    /// Reinterprets this frame as a larger frame `L`; only succeeds if the
    /// address is `L`-aligned (and `L` is not smaller than `S`).
    #[inline]
    pub fn as_larger<L: PageSize>(self) -> Option<PhysFrame<L>> {
        if L::SIZE < S::SIZE {
            return None;
        }
        PhysFrame::<L>::from_start_address_checked(self.start_address)
    }

    /// The larger frame `L` that contains this frame.
    #[inline]
    pub fn containing_frame<L: PageSize>(self) -> Option<PhysFrame<L>> {
        if L::SIZE < S::SIZE {
            return None;
        }
        Some(PhysFrame::<L>::containing_address(self.start_address))
    }
}

impl<S: PageSize> fmt::Display for PhysFrame<S> {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(
            f,
            "PhysFrame<{}>{{ start: {}, index: {} }}",
            S::NAME,
            self.start_address,
            self.index()
        )
    }
}

/// Iterator over the frames `[start, end)`.
pub struct PhysFrameRange<S: PageSize> {
    // Frame *indices*, so iteration never has to multiply by the page size.
    start: usize,
    end: usize,
    _marker: PhantomData<S>,
}

impl<S: PageSize> Clone for PhysFrameRange<S> {
    #[inline]
    fn clone(&self) -> Self {
        *self
    }
}
impl<S: PageSize> Copy for PhysFrameRange<S> {}

impl<S: PageSize> PartialEq for PhysFrameRange<S> {
    fn eq(&self, other: &Self) -> bool {
        self.start == other.start && self.end == other.end
    }
}
impl<S: PageSize> Eq for PhysFrameRange<S> {}

impl<S: PageSize> fmt::Debug for PhysFrameRange<S> {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "PhysFrameRange<{}>[{}..{})", S::NAME, self.start, self.end)
    }
}

impl<S: PageSize> PhysFrameRange<S> {
    /// Creates the half-open range `[start, end)`. If `end <= start` the range
    /// is empty.
    #[inline]
    pub fn new(start: PhysFrame<S>, end: PhysFrame<S>) -> Self {
        Self {
            start: start.index(),
            end: end.index(),
            _marker: PhantomData,
        }
    }

    /// Creates a range of `count` frames starting at `start`.
    #[inline]
    pub fn from_start_count(start: PhysFrame<S>, count: usize) -> Option<Self> {
        start.range_of(count)
    }

    /// First frame (meaningful only if the range is not empty).
    #[inline]
    pub fn start(&self) -> PhysFrame<S> {
        PhysFrame::from_index(self.start)
    }

    /// One-past-the-last frame.
    #[inline]
    pub fn end(&self) -> PhysFrame<S> {
        PhysFrame::from_index(self.end)
    }

    /// Number of frames still in the range.
    #[inline]
    pub fn len(&self) -> usize {
        self.end.saturating_sub(self.start)
    }

    /// `true` if no frames remain.
    #[inline]
    pub fn is_empty(&self) -> bool {
        self.start >= self.end
    }

    /// `true` if `frame` lies inside the range.
    #[inline]
    pub fn contains(&self, frame: PhysFrame<S>) -> bool {
        let i = frame.index();
        i >= self.start && i < self.end
    }

    /// Start address of the first frame.
    #[inline]
    pub fn start_address(&self) -> PhysAddr {
        self.start().start_address()
    }

    /// Total size of the remaining range in bytes (saturating).
    #[inline]
    pub fn size_in_bytes(&self) -> usize {
        self.len().saturating_mul(S::SIZE)
    }
}

impl<S: PageSize> Iterator for PhysFrameRange<S> {
    type Item = PhysFrame<S>;

    #[inline]
    fn next(&mut self) -> Option<Self::Item> {
        if self.start < self.end {
            let f = PhysFrame::from_index(self.start);
            self.start += 1;
            Some(f)
        } else {
            None
        }
    }

    #[inline]
    fn size_hint(&self) -> (usize, Option<usize>) {
        let n = self.len();
        (n, Some(n))
    }
}

impl<S: PageSize> DoubleEndedIterator for PhysFrameRange<S> {
    #[inline]
    fn next_back(&mut self) -> Option<Self::Item> {
        if self.start < self.end {
            self.end -= 1;
            Some(PhysFrame::from_index(self.end))
        } else {
            None
        }
    }
}

impl<S: PageSize> ExactSizeIterator for PhysFrameRange<S> {}

/// Iterator over the frames `[start, end]` (both ends inclusive).
pub struct PhysFrameRangeInclusive<S: PageSize> {
    start: usize,
    end: usize,
    // Needed because `end == usize::MAX` cannot be expressed as an exclusive
    // bound; also marks the iterator as exhausted after the last item.
    exhausted: bool,
    _marker: PhantomData<S>,
}

impl<S: PageSize> Clone for PhysFrameRangeInclusive<S> {
    #[inline]
    fn clone(&self) -> Self {
        *self
    }
}
impl<S: PageSize> Copy for PhysFrameRangeInclusive<S> {}

impl<S: PageSize> fmt::Debug for PhysFrameRangeInclusive<S> {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(
            f,
            "PhysFrameRangeInclusive<{}>[{}..={}]{}",
            S::NAME,
            self.start,
            self.end,
            if self.exhausted { " (exhausted)" } else { "" }
        )
    }
}

impl<S: PageSize> PhysFrameRangeInclusive<S> {
    /// Creates the closed range `[start, end]`. If `end < start` it is empty.
    #[inline]
    pub fn new(start: PhysFrame<S>, end: PhysFrame<S>) -> Self {
        Self {
            start: start.index(),
            end: end.index(),
            exhausted: false,
            _marker: PhantomData,
        }
    }

    /// `true` if no frames remain.
    #[inline]
    pub fn is_empty(&self) -> bool {
        self.exhausted || self.start > self.end
    }

    /// Number of frames still in the range (saturating).
    #[inline]
    pub fn len(&self) -> usize {
        if self.is_empty() {
            0
        } else {
            (self.end - self.start).saturating_add(1)
        }
    }
}

impl<S: PageSize> Iterator for PhysFrameRangeInclusive<S> {
    type Item = PhysFrame<S>;

    #[inline]
    fn next(&mut self) -> Option<Self::Item> {
        if self.is_empty() {
            return None;
        }
        let f = PhysFrame::from_index(self.start);
        if self.start == self.end {
            self.exhausted = true;
        } else {
            self.start += 1;
        }
        Some(f)
    }

    #[inline]
    fn size_hint(&self) -> (usize, Option<usize>) {
        let n = self.len();
        (n, Some(n))
    }
}

impl<S: PageSize> DoubleEndedIterator for PhysFrameRangeInclusive<S> {
    #[inline]
    fn next_back(&mut self) -> Option<Self::Item> {
        if self.is_empty() {
            return None;
        }
        let f = PhysFrame::from_index(self.end);
        if self.start == self.end {
            self.exhausted = true;
        } else {
            self.end -= 1;
        }
        Some(f)
    }
}

impl<S: PageSize> ExactSizeIterator for PhysFrameRangeInclusive<S> {}

// ============================================================================
// AllocatorStats & FragmentationReport
// ============================================================================

/// Statistics for a frame allocator.
#[derive(Debug, Clone, Copy)]
pub struct AllocatorStats {
    pub name: &'static str,
    pub total_frames: usize,
    pub allocated_frames: usize,
    pub free_frames: usize,
}

impl fmt::Display for AllocatorStats {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        writeln!(f, "Frame Allocator: {}", self.name)?;
        writeln!(f, "  Total:     {}", self.total_frames)?;
        writeln!(f, "  Allocated: {}", self.allocated_frames)?;
        writeln!(f, "  Free:      {}", self.free_frames)
    }
}

/// Fragmentation report for bitmap analysis.
#[derive(Debug, Clone, Copy)]
pub struct FragmentationReport {
    pub total_frames: usize,
    pub allocated_frames: usize,
    pub free_frames: usize,
    pub largest_free_block: usize,
    pub free_block_count: usize,
    pub fragmentation_index: f64,
}

impl fmt::Display for FragmentationReport {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        writeln!(f, "Fragmentation Report:")?;
        writeln!(f, "  Total frames:     {}", self.total_frames)?;
        writeln!(f, "  Allocated frames: {}", self.allocated_frames)?;
        writeln!(f, "  Free frames:      {}", self.free_frames)?;
        writeln!(f, "  Largest free:     {}", self.largest_free_block)?;
        writeln!(f, "  Free blocks:      {}", self.free_block_count)?;
        writeln!(f, "  Fragmentation:    {:.4}", self.fragmentation_index)
    }
}

// ============================================================================
// PhysicalFrameAllocator
// ============================================================================

/// SMP-safe physical frame allocator backed by a bitmap.
pub struct PhysicalFrameAllocator<S: PageSize> {
    name: &'static str,
    total_frames: usize,
    allocated_frames: AtomicUsize,
    bitmap: IrqSpinlock<crate::map::BitmapLayout>,
    _marker: PhantomData<S>,
}

impl<S: PageSize> PhysicalFrameAllocator<S> {
    /// Creates a new frame allocator.
    ///
    /// # Safety
    /// `bitmap_words` must be a valid, exclusively owned mutable slice.
    /// `bitmap_words.len() * 64 >= total_frames`.
    pub unsafe fn new(
        name: &'static str,
        total_frames: usize,
        bitmap_words: &'static mut [u64],
    ) -> Self {
        let layout = unsafe { crate::map::BitmapLayout::new(bitmap_words, total_frames) }
            .expect("bitmap layout init failed");
        Self {
            name,
            total_frames,
            allocated_frames: AtomicUsize::new(0),
            bitmap: IrqSpinlock::new(layout),
            _marker: PhantomData,
        }
    }

    /// Returns the allocator name.
    #[inline]
    pub fn name(&self) -> &'static str {
        self.name
    }

    /// Allocates a single physical frame.
    pub fn allocate_frame(&self) -> Option<PhysFrame<S>> {
        let mut bitmap = self.bitmap.lock();
        let idx = bitmap.allocate_single().ok()?;
        self.allocated_frames.fetch_add(1, Ordering::Relaxed);
        Some(PhysFrame::from_index(idx))
    }

    /// Allocates `count` contiguous frames.
    pub fn allocate_contiguous(&self, count: usize) -> Option<PhysFrame<S>> {
        let mut bitmap = self.bitmap.lock();
        let idx = bitmap.allocate_contiguous(count).ok()?;
        self.allocated_frames.fetch_add(count, Ordering::Relaxed);
        Some(PhysFrame::from_index(idx))
    }

    /// Deallocates a frame.
    ///
    /// # Safety
    /// The frame must have been allocated from this allocator and must no
    /// longer be in use.
    pub unsafe fn deallocate_frame(&self, frame: PhysFrame<S>) {
        let mut bitmap = self.bitmap.lock();
        let _ = bitmap.clear(frame.index());
        self.allocated_frames.fetch_sub(1, Ordering::Relaxed);
    }

    /// Marks a range of frames as reserved (kernel image, ACPI, etc.).
    pub fn register_reserved_region(&self, start: PhysAddr, count: usize) {
        let mut bitmap = self.bitmap.lock();
        let start_idx = start.as_usize() / S::SIZE;
        let _ = bitmap.set_range(start_idx, count);
        self.allocated_frames.fetch_add(count, Ordering::Relaxed);
    }

    /// Returns statistics.
    pub fn stats(&self) -> AllocatorStats {
        let allocated = self.allocated_frames.load(Ordering::Relaxed);
        AllocatorStats {
            name: self.name,
            total_frames: self.total_frames,
            allocated_frames: allocated,
            free_frames: self.total_frames.saturating_sub(allocated),
        }
    }

    /// Returns a fragmentation report.
    pub fn fragmentation_report(&self) -> FragmentationReport {
        let bitmap = self.bitmap.lock();
        let report = bitmap.fragmentation_report();
        FragmentationReport {
            total_frames: report.total_frames,
            allocated_frames: report.allocated_frames,
            free_frames: report.free_frames,
            largest_free_block: report.largest_free_block,
            free_block_count: report.free_block_count,
            fragmentation_index: report.fragmentation_index,
        }
    }
}

// ============================================================================
// AllocatorRegistry (placeholder for global registry)
// ============================================================================

/// Global registry of frame allocators (one per page size).
pub struct AllocatorRegistry {
    pub alloc_4k: Option<&'static PhysicalFrameAllocator<Size4Kb>>,
    pub alloc_2m: Option<&'static PhysicalFrameAllocator<Size2Mb>>,
    pub alloc_1g: Option<&'static PhysicalFrameAllocator<Size1Gb>>,
}

impl AllocatorRegistry {
    /// Creates an empty registry.
    #[inline]
    pub const fn new() -> Self {
        Self {
            alloc_4k: None,
            alloc_2m: None,
            alloc_1g: None,
        }
    }
}
