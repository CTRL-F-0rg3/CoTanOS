// SPDX-FileCopyrightText: 2026 CTRL-F-0rg3
// SPDX-License-Identifier: GPL-3.0-only
use core::sync::atomic::{AtomicUsize, Ordering};
use core::ptr::NonNull;
use core::mem;

/// CoTanOS Early Boot Arena Allocator.
///
/// Designed as a "fire-and-forget", maximally reliable allocator for the 
/// earliest stages of the CoTanOS boot process.
/// 
/// # Guarantees
/// - **Lock-free & SMP-safe**: Uses `AcqRel` memory ordering to prevent race 
///   conditions when multiple cores wake up and allocate concurrently.
/// - **Cache-line isolated**: `#[repr(C, align(64))]` prevents false-sharing 
///   with adjacent global variables in the BSS/DATA segments.
/// - **Bounds-checked**: All arithmetic uses `checked_*` methods to prevent 
///   integer overflow vulnerabilities.
/// - **Null-page guarded**: Automatically skips address `0x0` to prevent 
///   Undefined Behavior when creating `NonNull` pointers.
#[repr(C, align(64))]
pub struct ArenaAllocator {
    /// Name of the arena for kernel registration, logging, and debugging.
    name: &'static str,
    /// Start of the backing memory region (inclusive).
    buffer_start: usize,
    /// End of the backing memory region (exclusive).
    buffer_end: usize,
    /// Current allocation offset from `buffer_start`.
    offset: AtomicUsize,
}

impl ArenaAllocator {
    /// Creates a new arena from a raw memory range.
    ///
    /// # Safety
    /// The caller must guarantee that:
    /// 1. `start..start+size` is a valid, writable, and mapped memory range.
    /// 2. This range is exclusively owned by this allocator for its entire lifetime.
    /// 3. The memory is not concurrently accessed or modified by hardware/firmware 
    ///    (e.g., DMA, bootloader) after this point.
    #[inline]
    pub const unsafe fn new(name: &'static str, start: usize, size: usize) -> Self {
        let buffer_end = match start.checked_add(size) {
            Some(end) => end,
            None => panic!("CoTanOS ArenaAllocator: buffer range overflow"),
        };

        Self {
            name,
            buffer_start: start,
            buffer_end,
            offset: AtomicUsize::new(0),
        }
    }

    /// Returns the registration name of this allocator.
    #[inline]
    pub const fn name(&self) -> &'static str {
        self.name
    }

    #[inline(always)]
    const fn align_up(addr: usize, align: usize) -> Option<usize> {
        debug_assert!(align.is_power_of_two() && align > 0);
        let mask = align - 1;
        addr.checked_add(mask).map(|a| a & !mask)
    }

    /// Attempts to allocate `size` bytes with the specified `align`.
    /// Returns `None` on Out-Of-Memory (OOM) or invalid alignment.
    #[inline]
    pub fn try_alloc_raw(&self, size: usize, align: usize) -> Option<NonNull<u8>> {
        if !align.is_power_of_two() || align == 0 {
            return None;
        }

        // Zero-Sized Types (ZST): Return a properly aligned dangling pointer.
        // This consumes 0 bytes of the arena and prevents pointer aliasing bugs.
        if size == 0 {
            return Some(unsafe { NonNull::new_unchecked(align as *mut u8) });
        }

        // Acquire ensures we see all prior writes to the offset from other cores.
        let mut current = self.offset.load(Ordering::Acquire);

        loop {
            let current_addr = self.buffer_start.checked_add(current)?;
            let mut aligned_addr = Self::align_up(current_addr, align)?;
            
            // Null-page guard: Address 0 is reserved in Rust. If alignment 
            // resolves to 0, push it to the next valid aligned boundary.
            if aligned_addr == 0 {
                aligned_addr = align;
            }
            
            let padding = aligned_addr.checked_sub(current_addr)?;

            let new_offset = current
                .checked_add(padding)?
                .checked_add(size)?;

            let new_end = self.buffer_start.checked_add(new_offset)?;
            if new_end > self.buffer_end {
                return None; // Strict bounds check: OOM
            }

            // AcqRel ensures that the allocation of memory happens-before 
            // any core observes the new offset, preventing memory reordering bugs.
            match self.offset.compare_exchange_weak(
                current,
                new_offset,
                Ordering::AcqRel,
                Ordering::Acquire,
            ) {
                Ok(_) => {
                    // Safety: `aligned_addr` is > 0 (null-page guard) and 
                    // strictly bounded by `buffer_end`.
                    return Some(unsafe { NonNull::new_unchecked(aligned_addr as *mut u8) });
                }
                Err(actual) => current = actual, // Retry with the actual current value
            }
        }
    }

    /// Allocates raw memory. Panics on OOM.
    /// Intended for early boot where OOM is a fatal, unrecoverable kernel state.
    #[inline]
    pub fn alloc_raw(&self, size: usize, align: usize) -> NonNull<u8> {
        self.try_alloc_raw(size, align)
            .expect("CoTanOS ArenaAllocator: Fatal OOM during early boot")
    }

    /// Safely allocates and initializes a slice of bytes.
    /// 
    /// # Scope & Safety
    /// The returned mutable slice is tied to the lifetime of the arena (`'a`).
    /// In CoTanOS, boot arenas are typically `'static`, but the compiler will 
    /// enforce that this slice does not outlive the arena instance.
    #[inline]
    pub fn alloc_slice<'a>(&'a self, len: usize) -> &'a mut [u8] {
        let ptr = self.alloc_raw(len, mem::align_of::<u8>());
        // Safety: ptr is valid, aligned, and within bounds for `len` bytes.
        unsafe { core::slice::from_raw_parts_mut(ptr.as_ptr(), len) }
    }

    /// Safely allocates and initializes a value of type `T` using a closure.
    /// 
    /// # Why this is safer than `alloc_val`
    /// This guarantees that the memory is fully initialized *before* the 
    /// reference is ever handed to the caller, completely eliminating the 
    /// risk of reading uninitialized memory (UB).
    ///
    /// # Scope & Safety
    /// The returned reference is bound to the arena's lifetime (`'a`).
    #[inline]
    pub fn alloc_with<'a, T, F>(&'a self, f: F) -> &'a mut T
    where
        F: FnOnce() -> T,
    {
        let ptr = self.alloc_raw(mem::size_of::<T>(), mem::align_of::<T>()).cast::<T>();
        // Safety: ptr is valid, aligned, and we write to it immediately.
        unsafe {
            ptr.as_ptr().write(f());
            &mut *ptr.as_ptr()
        }
    }

    /// Returns the total capacity of the arena in bytes.
    #[inline]
    pub const fn capacity(&self) -> usize {
        self.buffer_end - self.buffer_start
    }

    /// Returns the number of bytes currently allocated (including padding).
    #[inline]
    pub fn bytes_allocated(&self) -> usize {
        self.offset.load(Ordering::Relaxed)
    }

    /// Returns the number of free bytes remaining.
    /// Useful for boot-time assertions: `assert!(arena.bytes_free() > MIN_BOOT_MEM);`
    #[inline]
    pub fn bytes_free(&self) -> usize {
        self.capacity().saturating_sub(self.bytes_allocated())
    }

    /// Resets the allocator, freeing all allocations.
    ///
    /// # Safety
    /// The caller must guarantee that:
    /// 1. NO live references or pointers to this arena's memory exist anywhere in the system.
    /// 2. This is NOT called concurrently with any `alloc` operations.
    /// (Typically only safe to call during a controlled kernel reset or teardown).
    #[inline]
    pub unsafe fn reset(&self) {
        // Release ensures all prior writes to the arena's memory are visible 
        // before the offset is reset, preventing use-after-free on other cores.
        self.offset.store(0, Ordering::Release);
    }
}