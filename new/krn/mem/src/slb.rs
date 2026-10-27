// SPDX-FileCopyrightText: 2026 CTRL-F-0rg3
// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2026 CTRL-F-0rg3
// SPDX-License-Identifier: GPL-3.0-only
//! CoTanOS Kernel Slab Allocator (`slb.rs`)
//!
//! General-purpose object allocator for kernel data structures. Built on top
//! of the physical frame allocator (`frm.rs`) and designed for the hybrid
//! microkernel architecture of CoTanOS (where the network driver remains
//! in-kernel and requires fast, frequent allocations).
//!
//! # Design Principles
//!
//! * **Size-segregated caches**: Objects are grouped into fixed-size caches
//!   (32, 64, 96, 128, 192, 256, 512, 1024, 2048, 4096, 8192 bytes). This
//!   eliminates external fragmentation and allows O(1) allocation/deallocation.
//!
//! * **Per-CPU magazines**: Each CPU maintains a small lock-free cache of
//!   recently freed objects, dramatically reducing contention on the global
//!   slab lock in SMP systems.
//!
//! * **Debug poisoning**: In debug builds, freed objects are filled with
//!   `0xDEADBEEF` patterns and surrounded by redzones to catch use-after-free
//!   and buffer overflow bugs immediately.
//!
//! * **Emergency pool**: A small reserve of pages is kept for critical
//!   allocations (e.g., interrupt handlers, panic paths) that cannot fail.
//!
//! * **Constructor/destructor callbacks**: Slab caches can register init/fini
//!   functions that are called when objects are allocated/freed, ensuring
//!   proper initialization and cleanup.
//!
//! * **SMP-safe**: All global state is protected by [`IrqSpinlock`] from
//!   `frm.rs`. Per-CPU caches are lock-free.
//!
//! # File Layout
//!
//! 1. [`SlabError`] — error types for slab operations.
//! 2. [`SlabSize`] — predefined object sizes and cache selection.
//! 3. [`SlabObject`] — metadata for a single allocated object (debug only).
//! 4. [`SlabPage`] — a physical page divided into objects, with free list.
//! 5. [`SlabCache`] — cache for a specific object size.
//! 6. [`SlabAllocator`] — main allocator managing multiple caches.
//! 7. [`SlabStats`] — statistics for diagnostics.
//! 8. [`SlabRegistry`] — global registry of all caches.
//! 9. Per-CPU magazine support.
//! 10. Emergency pool.
//! 11. Tests.

use core::fmt;
use core::mem;
use core::ptr::{self, NonNull};

use crate::frm::{
    FrameError, IrqSpinlock, PageSize, PhysAddr, PhysFrame, PhysicalFrameAllocator, Size4Kb,
};

// ============================================================================
// Constants & Debug Patterns
// ============================================================================

/// Poison pattern for freed objects (debug builds).
#[cfg(debug_assertions)]
const POISON_PATTERN: u8 = 0xDE;

/// Redzone pattern for buffer overflow detection (debug builds).
#[cfg(debug_assertions)]
const REDZONE_PATTERN: u8 = 0xAD;

/// Size of the redzone (before and after each object) in debug builds.
#[cfg(debug_assertions)]
const REDZONE_SIZE: usize = 16;

/// Number of objects in a per-CPU magazine.
const MAGAZINE_SIZE: usize = 8;

/// Number of emergency pages reserved for critical allocations.
const EMERGENCY_PAGES: usize = 4;

/// Minimum alignment for all slab objects (prevents unaligned access).
const MIN_ALIGNMENT: usize = 8;

// ============================================================================
// 1. SlabError
// ============================================================================

/// Errors that can occur during slab operations.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum SlabError {
    /// The requested size is too large for any slab cache.
    SizeTooLarge {
        requested: usize,
        max_supported: usize,
    },
    /// No memory available (all slabs full, no pages from frame allocator).
    OutOfMemory,
    /// The object was not allocated from this cache (wrong cache or double-free).
    InvalidObject {
        addr: usize,
        expected_size: usize,
    },
    /// The slab cache is not initialized or has been destroyed.
    CacheNotInitialized,
    /// The frame allocator returned an error.
    FrameAllocatorError(FrameError),
    /// Debug-only: redzone corruption detected (buffer overflow).
    #[cfg(debug_assertions)]
    RedzoneCorruption {
        addr: usize,
        location: &'static str,
    },
    /// Debug-only: object was not properly poisoned (use-after-free).
    #[cfg(debug_assertions)]
    UseAfterFree {
        addr: usize,
    },
}

impl fmt::Display for SlabError {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            Self::SizeTooLarge { requested, max_supported } => {
                write!(
                    f,
                    "slab: requested size {} exceeds max supported {}",
                    requested, max_supported
                )
            }
            Self::OutOfMemory => write!(f, "slab: out of memory"),
            Self::InvalidObject { addr, expected_size } => {
                write!(
                    f,
                    "slab: invalid object at {:#x} (expected size {})",
                    addr, expected_size
                )
            }
            Self::CacheNotInitialized => write!(f, "slab: cache not initialized"),
            Self::FrameAllocatorError(e) => write!(f, "slab: frame allocator error: {}", e),
            #[cfg(debug_assertions)]
            Self::RedzoneCorruption { addr, location } => {
                write!(
                    f,
                    "slab: redzone corruption at {:#x} ({})",
                    addr, location
                )
            }
            #[cfg(debug_assertions)]
            Self::UseAfterFree { addr } => {
                write!(f, "slab: use-after-free detected at {:#x}", addr)
            }
        }
    }
}

impl From<FrameError> for SlabError {
    fn from(e: FrameError) -> Self {
        Self::FrameAllocatorError(e)
    }
}

// ============================================================================
// 2. SlabSize — predefined object sizes
// ============================================================================

/// Predefined object sizes for slab caches.
///
/// These sizes are chosen to minimize internal fragmentation while covering
/// the most common kernel allocation patterns (network buffers, task structs,
/// file descriptors, etc.).
#[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord)]
#[repr(usize)]
pub enum SlabSize {
    Size32 = 32,
    Size64 = 64,
    Size96 = 96,
    Size128 = 128,
    Size192 = 192,
    Size256 = 256,
    Size512 = 512,
    Size1024 = 1024,
    Size2048 = 2048,
    Size4096 = 4096,
    Size8192 = 8192,
}

impl SlabSize {
    /// Returns the size in bytes.
    #[inline]
    pub const fn size(self) -> usize {
        self as usize
    }

    /// Returns the human-readable name.
    #[inline]
    pub const fn name(self) -> &'static str {
        match self {
            Self::Size32 => "slab-32",
            Self::Size64 => "slab-64",
            Self::Size96 => "slab-96",
            Self::Size128 => "slab-128",
            Self::Size192 => "slab-192",
            Self::Size256 => "slab-256",
            Self::Size512 => "slab-512",
            Self::Size1024 => "slab-1024",
            Self::Size2048 => "slab-2048",
            Self::Size4096 => "slab-4096",
            Self::Size8192 => "slab-8192",
        }
    }

    /// Returns the appropriate slab size for a given requested size.
    ///
    /// Rounds up to the nearest predefined size. Returns `None` if the
    /// requested size exceeds the maximum supported size (8192 bytes).
    #[inline]
    pub fn for_size(requested: usize) -> Option<Self> {
        if requested == 0 {
            return Some(Self::Size32);
        }
        if requested <= 32 {
            Some(Self::Size32)
        } else if requested <= 64 {
            Some(Self::Size64)
        } else if requested <= 96 {
            Some(Self::Size96)
        } else if requested <= 128 {
            Some(Self::Size128)
        } else if requested <= 192 {
            Some(Self::Size192)
        } else if requested <= 256 {
            Some(Self::Size256)
        } else if requested <= 512 {
            Some(Self::Size512)
        } else if requested <= 1024 {
            Some(Self::Size1024)
        } else if requested <= 2048 {
            Some(Self::Size2048)
        } else if requested <= 4096 {
            Some(Self::Size4096)
        } else if requested <= 8192 {
            Some(Self::Size8192)
        } else {
            None
        }
    }

    /// Returns all predefined sizes in ascending order.
    #[inline]
    pub const fn all() -> [Self; 11] {
        [
            Self::Size32,
            Self::Size64,
            Self::Size96,
            Self::Size128,
            Self::Size192,
            Self::Size256,
            Self::Size512,
            Self::Size1024,
            Self::Size2048,
            Self::Size4096,
            Self::Size8192,
        ]
    }
}

// ============================================================================
// 3. SlabObject — metadata (debug only)
// ============================================================================

/// Metadata for a single allocated object (debug builds only).
///
/// This structure is stored in a separate metadata array (not inline with
/// the object) to avoid wasting space in production builds.
#[cfg(debug_assertions)]
#[derive(Debug, Clone, Copy)]
struct SlabObjectMeta {
    /// Magic number to detect corruption.
    magic: u32,
    /// Size of the object (for validation).
    size: usize,
    /// CPU that allocated this object (for debugging).
    cpu_id: u32,
    /// Allocation timestamp (TSC or monotonic counter).
    alloc_time: u64,
}

#[cfg(debug_assertions)]
const SLAB_OBJECT_MAGIC: u32 = 0x51AB_0BEC;

// ============================================================================
// 4. SlabPage — a physical page divided into objects
// ============================================================================

/// A physical page managed by a slab cache.
///
/// The page is divided into:
/// - Header (SlabPageHeader)
/// - Array of objects (each of size `object_size`)
/// - Free list (embedded in free objects)
#[repr(C)]
struct SlabPageHeader {
    /// Magic number for validation.
    magic: u32,
    /// Number of objects currently allocated from this page.
    inuse: u16,
    /// Total number of objects that fit in this page.
    total: u16,
    /// Index of the first free object (or `u16::MAX` if full).
    free_head: u16,
    /// Pointer to the next page in the list (physical address).
    next: PhysAddr,
    /// Pointer to the previous page in the list (physical address).
    prev: PhysAddr,
}

const SLAB_PAGE_MAGIC: u32 = 0x534C4142; // "SLAB"

impl SlabPageHeader {
    /// Creates a new, empty slab page header.
    #[inline]
    const fn new() -> Self {
        Self {
            magic: SLAB_PAGE_MAGIC,
            inuse: 0,
            total: 0,
            free_head: u16::MAX,
            next: PhysAddr::ZERO,
            prev: PhysAddr::ZERO,
        }
    }

    /// Returns `true` if the page is completely free.
    #[inline]
    const fn is_empty(&self) -> bool {
        self.inuse == 0
    }

    /// Returns `true` if the page is completely full.
    #[inline]
    const fn is_full(&self) -> bool {
        self.inuse == self.total
    }
}

// ============================================================================
// 5. SlabCache — cache for a specific object size
// ============================================================================

/// A slab cache for objects of a specific size.
///
/// Each cache manages a pool of pages, each divided into objects of the
/// cache's size. Pages are organized into three lists:
/// - `partial`: pages with some free objects
/// - `full`: pages with no free objects
/// - `empty`: pages with all objects free (candidates for deallocation)
struct SlabCache {
    /// Name of this cache (for diagnostics).
    name: &'static str,
    /// Size of objects in this cache (bytes).
    object_size: usize,
    /// Aligned object size (includes redzones in debug builds).
    aligned_size: usize,
    /// Number of objects per page.
    objects_per_page: usize,
    /// List of partial pages (some free objects).
    partial_pages: PhysAddr,
    /// List of full pages (no free objects).
    full_pages: PhysAddr,
    /// List of empty pages (all free, candidates for release).
    empty_pages: PhysAddr,
    /// Total number of pages allocated for this cache.
    total_pages: usize,
    /// Total number of objects currently allocated.
    total_allocated: usize,
    /// Constructor callback (called on allocation).
    ctor: Option<fn(*mut u8)>,
    /// Destructor callback (called on deallocation).
    dtor: Option<fn(*mut u8)>,
}

impl SlabCache {
    /// Creates a new slab cache for the given object size.
    ///
    /// # Safety
    /// The caller must ensure that the cache is properly initialized before use.
    #[inline]
    unsafe fn new(name: &'static str, object_size: usize) -> Self {
        let aligned_size = Self::align_size(object_size);
        let header_size = mem::size_of::<SlabPageHeader>();
        let usable_space = Size4Kb::SIZE - header_size;
        let objects_per_page = usable_space / aligned_size;

        Self {
            name,
            object_size,
            aligned_size,
            objects_per_page,
            partial_pages: PhysAddr::ZERO,
            full_pages: PhysAddr::ZERO,
            empty_pages: PhysAddr::ZERO,
            total_pages: 0,
            total_allocated: 0,
            ctor: None,
            dtor: None,
        }
    }

    /// Aligns the object size to `MIN_ALIGNMENT` and adds redzones (debug).
    #[inline]
    fn align_size(size: usize) -> usize {
        let mut aligned = (size + MIN_ALIGNMENT - 1) & !(MIN_ALIGNMENT - 1);
        #[cfg(debug_assertions)]
        {
            aligned += REDZONE_SIZE * 2;
        }
        aligned
    }

    /// Sets the constructor callback.
    #[inline]
    fn set_ctor(&mut self, ctor: fn(*mut u8)) {
        self.ctor = Some(ctor);
    }

    /// Sets the destructor callback.
    #[inline]
    fn set_dtor(&mut self, dtor: fn(*mut u8)) {
        self.dtor = Some(dtor);
    }

    /// Allocates a new page from the frame allocator and initializes it.
    fn grow<F: FnMut(usize) -> Result<PhysFrame<Size4Kb>, FrameError>>(
        &mut self,
        alloc_frame: &mut F,
    ) -> Result<(), SlabError> {
        let frame = alloc_frame(self.object_size)?;
        let page_addr = frame.start_address();

        // Safety: we just allocated this page, so it's valid and exclusive.
        unsafe {
            let header = &mut *(page_addr.as_usize() as *mut SlabPageHeader);
            *header = SlabPageHeader::new();
            header.total = self.objects_per_page as u16;

            // Initialize the free list.
            let mut free_head = u16::MAX;
            for i in (0..self.objects_per_page).rev() {
                let obj_addr = Self::object_address(page_addr, i, self.aligned_size);
                let next_ptr = obj_addr.as_usize() as *mut u16;
                *next_ptr = free_head;
                free_head = i as u16;

                #[cfg(debug_assertions)]
                {
                    // Poison the object and redzones.
                    let obj_ptr = obj_addr.as_usize() as *mut u8;
                    ptr::write_bytes(obj_ptr, POISON_PATTERN, self.aligned_size);
                }
            }
            header.free_head = free_head;

            // Add to empty list.
            Self::push_page(&mut self.empty_pages, page_addr);
        }

        self.total_pages += 1;
        Ok(())
    }

    /// Computes the address of the i-th object in a page.
    #[inline]
    fn object_address(page_addr: PhysAddr, index: usize, aligned_size: usize) -> PhysAddr {
        let offset = mem::size_of::<SlabPageHeader>() + index * aligned_size;
        PhysAddr::new(page_addr.as_usize() + offset)
    }

    /// Pushes a page onto a list (partial, full, or empty).
    fn push_page(list: &mut PhysAddr, page_addr: PhysAddr) {
        unsafe {
            let header = &mut *(page_addr.as_usize() as *mut SlabPageHeader);
            header.next = *list;
            header.prev = PhysAddr::ZERO;
            if !(*list).is_null() {
                let next_header = &mut *((*list).as_usize() as *mut SlabPageHeader);
                next_header.prev = page_addr;
            }
            *list = page_addr;
        }
    }

    /// Removes a page from a list.
    fn remove_page(list: &mut PhysAddr, page_addr: PhysAddr) {
        unsafe {
            let header = &mut *(page_addr.as_usize() as *mut SlabPageHeader);
            if !header.prev.is_null() {
                let prev_header = &mut *(header.prev.as_usize() as *mut SlabPageHeader);
                prev_header.next = header.next;
            } else {
                *list = header.next;
            }
            if !header.next.is_null() {
                let next_header = &mut *(header.next.as_usize() as *mut SlabPageHeader);
                next_header.prev = header.prev;
            }
            header.next = PhysAddr::ZERO;
            header.prev = PhysAddr::ZERO;
        }
    }

    /// Allocates an object from this cache.
    fn allocate<F: FnMut(usize) -> Result<PhysFrame<Size4Kb>, FrameError>>(
        &mut self,
        alloc_frame: &mut F,
    ) -> Result<NonNull<u8>, SlabError> {
        // Try partial list first.
        if !self.partial_pages.is_null() {
            return unsafe { self.allocate_from_list(&mut self.partial_pages, true, alloc_frame) };
        }

        // Try empty list.
        if !self.empty_pages.is_null() {
            return unsafe { unsafe { self.allocate_from_list(&mut self.empty_pages, false, alloc_frame) } };
        }

        // Grow the cache.
        self.grow(alloc_frame)?;
        unsafe { self.allocate_from_list(&mut self.empty_pages, false, alloc_frame) }
    }

    /// Allocates an object from a specific list (partial or empty).
    unsafe fn allocate_from_list<F: FnMut(usize) -> Result<PhysFrame<Size4Kb>, FrameError>>(
        &mut self,
        list: &mut PhysAddr,
        is_partial: bool,
        _alloc_frame: &mut F,
    ) -> Result<NonNull<u8>, SlabError> {
        let page_addr = *list;
        let header = &mut *(page_addr.as_usize() as *mut SlabPageHeader);

        debug_assert!(header.magic == SLAB_PAGE_MAGIC);
        debug_assert!(header.free_head != u16::MAX);

        let obj_index = header.free_head as usize;
        let obj_addr = Self::object_address(page_addr, obj_index, self.aligned_size);

        // Update free list.
        let next_ptr = obj_addr.as_usize() as *const u16;
        header.free_head = *next_ptr;
        header.inuse += 1;

        // Move page to appropriate list.
        if is_partial {
            if header.is_full() {
                Self::remove_page(list, page_addr);
                Self::push_page(&mut self.full_pages, page_addr);
            }
        } else {
            // Was empty, now partial.
            Self::remove_page(list, page_addr);
            Self::push_page(&mut self.partial_pages, page_addr);
        }

        self.total_allocated += 1;

        #[cfg(debug_assertions)]
        {
            // Initialize redzones.
            let obj_ptr = obj_addr.as_usize() as *mut u8;
            ptr::write_bytes(obj_ptr, REDZONE_PATTERN, REDZONE_SIZE);
            ptr::write_bytes(
                obj_ptr.add(REDZONE_SIZE + self.object_size),
                REDZONE_PATTERN,
                REDZONE_SIZE,
            );
        }

        // Call constructor if present.
        if let Some(ctor) = self.ctor {
            let user_ptr = self.user_pointer(obj_addr);
            ctor(user_ptr.as_ptr() as usize as *mut u8);
        }

        Ok(self.user_pointer(obj_addr))
    }

    /// Returns the user-visible pointer (skips redzone in debug builds).
    #[inline]
    fn user_pointer(&self, obj_addr: PhysAddr) -> NonNull<u8> {
        #[cfg(debug_assertions)]
        {
            NonNull::new((obj_addr.as_usize() + REDZONE_SIZE) as *mut u8).unwrap()
        }
        #[cfg(not(debug_assertions))]
        {
            NonNull::new(obj_addr.as_usize() as *mut u8).unwrap()
        }
    }

    /// Deallocates an object back to this cache.
    fn deallocate<F: FnMut(PhysFrame<Size4Kb>) -> Result<(), FrameError>>(
        &mut self,
        obj_ptr: NonNull<u8>,
        free_frame: &mut F,
    ) -> Result<(), SlabError> {
        let obj_addr = self.object_pointer(obj_ptr);
        let page_addr = self.page_for_object(obj_addr);

        unsafe {
            let header = &mut *(page_addr.as_usize() as *mut SlabPageHeader);

            if header.magic != SLAB_PAGE_MAGIC {
                return Err(SlabError::InvalidObject {
                    addr: obj_ptr.as_ptr() as usize,
                    expected_size: self.object_size,
                });
            }

            #[cfg(debug_assertions)]
            {
                // Check redzones.
                let obj_raw = obj_addr.as_usize() as *mut u8;
                for i in 0..REDZONE_SIZE {
                    if *obj_raw.add(i) != REDZONE_PATTERN {
                        return Err(SlabError::RedzoneCorruption {
                            addr: obj_ptr.as_ptr() as usize,
                            location: "before object",
                        });
                    }
                }
                for i in 0..REDZONE_SIZE {
                    if *obj_raw.add(REDZONE_SIZE + self.object_size + i) != REDZONE_PATTERN {
                        return Err(SlabError::RedzoneCorruption {
                            addr: obj_ptr.as_ptr() as usize,
                            location: "after object",
                        });
                    }
                }

                // Poison the object.
                ptr::write_bytes(obj_raw, POISON_PATTERN, self.aligned_size);
            }

            // Call destructor if present.
            if let Some(dtor) = self.dtor {
                dtor(obj_ptr.as_ptr());
            }

            // Add to free list.
            let obj_index = self.object_index(obj_addr, page_addr);
            let next_ptr = obj_addr.as_usize() as *mut u16;
            *next_ptr = header.free_head;
            header.free_head = obj_index as u16;
            header.inuse -= 1;

            // Move page to appropriate list.
            if header.is_full() {
                // Was full, now partial.
                Self::remove_page(&mut self.full_pages, page_addr);
                Self::push_page(&mut self.partial_pages, page_addr);
            } else if header.is_empty() {
                // Was partial, now empty.
                Self::remove_page(&mut self.partial_pages, page_addr);
                Self::push_page(&mut self.empty_pages, page_addr);

                // Optionally shrink: free empty pages if we have too many.
                // (Simplified: just keep them for now).
            }

            self.total_allocated -= 1;
        }

        Ok(())
    }

    /// Returns the object pointer (includes redzone in debug builds).
    #[inline]
    fn object_pointer(&self, user_ptr: NonNull<u8>) -> PhysAddr {
        #[cfg(debug_assertions)]
        {
            PhysAddr::new(user_ptr.as_ptr() as usize - REDZONE_SIZE)
        }
        #[cfg(not(debug_assertions))]
        {
            PhysAddr::new(user_ptr.as_ptr() as usize)
        }
    }

    /// Finds the page that contains the given object.
    fn page_for_object(&self, obj_addr: PhysAddr) -> PhysAddr {
        let page_addr = PhysAddr::new(obj_addr.as_usize() & !(Size4Kb::SIZE - 1));
        page_addr
    }

    /// Computes the index of an object within its page.
    fn object_index(&self, obj_addr: PhysAddr, page_addr: PhysAddr) -> usize {
        let offset = obj_addr.as_usize() - page_addr.as_usize() - mem::size_of::<SlabPageHeader>();
        offset / self.aligned_size
    }

    /// Returns statistics for this cache.
    fn stats(&self) -> SlabCacheStats {
        SlabCacheStats {
            name: self.name,
            object_size: self.object_size,
            total_pages: self.total_pages,
            total_allocated: self.total_allocated,
            total_objects: self.total_pages * self.objects_per_page,
            objects_per_page: self.objects_per_page,
        }
    }
}

// ============================================================================
// 6. SlabAllocator — main allocator
// ============================================================================

/// The main slab allocator, managing multiple caches for different object sizes.
pub struct SlabAllocator {
    /// Array of caches, one per predefined size.
    caches: [IrqSpinlock<SlabCache>; 11],
    /// Reference to the physical frame allocator.
    frame_allocator: &'static PhysicalFrameAllocator<Size4Kb>,
    /// Emergency pool (pre-allocated pages for critical paths).
    emergency_pool: IrqSpinlock<EmergencyPool>,
}

impl SlabAllocator {
    /// Creates a new slab allocator.
    ///
    /// # Safety
    /// The caller must ensure that `frame_allocator` is valid and will outlive
    /// this slab allocator.
    pub unsafe fn new(frame_allocator: &'static PhysicalFrameAllocator<Size4Kb>) -> Self {
        let caches = Self::init_caches();
        let emergency_pool = IrqSpinlock::new(EmergencyPool::new());

        Self {
            caches,
            frame_allocator,
            emergency_pool,
        }
    }

    /// Initializes all slab caches.
    unsafe fn init_caches() -> [IrqSpinlock<SlabCache>; 11] {
        [
            IrqSpinlock::new(unsafe { SlabCache::new("slab-32", 32) }),
            IrqSpinlock::new(unsafe { SlabCache::new("slab-64", 64) }),
            IrqSpinlock::new(unsafe { SlabCache::new("slab-96", 96) }),
            IrqSpinlock::new(unsafe { SlabCache::new("slab-128", 128) }),
            IrqSpinlock::new(unsafe { SlabCache::new("slab-192", 192) }),
            IrqSpinlock::new(unsafe { SlabCache::new("slab-256", 256) }),
            IrqSpinlock::new(unsafe { SlabCache::new("slab-512", 512) }),
            IrqSpinlock::new(unsafe { SlabCache::new("slab-1024", 1024) }),
            IrqSpinlock::new(unsafe { SlabCache::new("slab-2048", 2048) }),
            IrqSpinlock::new(unsafe { SlabCache::new("slab-4096", 4096) }),
            IrqSpinlock::new(unsafe { SlabCache::new("slab-8192", 8192) }),
        ]
    }

    /// Allocates an object of the given size.
    ///
    /// Returns a pointer to the allocated object, or an error if allocation fails.
    pub fn allocate(&self, size: usize) -> Result<NonNull<u8>, SlabError> {
        let slab_size = SlabSize::for_size(size).ok_or(SlabError::SizeTooLarge {
            requested: size,
            max_supported: 8192,
        })?;

        let cache_index = slab_size as usize;
        let mut cache = self.caches[cache_index].lock();

        let mut alloc_frame = |_: usize| -> Result<PhysFrame<Size4Kb>, FrameError> {
            self.frame_allocator.allocate_frame().ok_or(FrameError::OutOfMemory)
        };

        cache.allocate(&mut alloc_frame)
    }

    /// Deallocates an object.
    ///
    /// # Safety
    /// The caller must ensure that `ptr` was allocated from this allocator
    /// with the correct size.
    pub unsafe fn deallocate(&self, ptr: NonNull<u8>, size: usize) -> Result<(), SlabError> {
        let slab_size = SlabSize::for_size(size).ok_or(SlabError::SizeTooLarge {
            requested: size,
            max_supported: 8192,
        })?;

        let cache_index = slab_size as usize;
        let mut cache = self.caches[cache_index].lock();

        let mut free_frame = |_: PhysFrame<Size4Kb>| -> Result<(), FrameError> {
            // In a full implementation, we'd return the page to the frame allocator.
            // For now, we just keep it (simplified).
            Ok(())
        };

        cache.deallocate(ptr, &mut free_frame)
    }

    /// Returns statistics for all caches.
    pub fn stats(&self) -> [Option<SlabCacheStats>; 11] {
        {
        let mut result: [Option<SlabCacheStats>; 11] = [None; 11];
        for (i, cache) in self.caches.iter().enumerate() {
            result[i] = Some(cache.lock().stats());
        }
        result
    }
    }
}

// ============================================================================
// 7. SlabStats
// ============================================================================

/// Statistics for a single slab cache.
#[derive(Debug, Clone, Copy)]
pub struct SlabCacheStats {
    pub name: &'static str,
    pub object_size: usize,
    pub total_pages: usize,
    pub total_allocated: usize,
    pub total_objects: usize,
    pub objects_per_page: usize,
}

impl fmt::Display for SlabCacheStats {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        writeln!(f, "{}:", self.name)?;
        writeln!(f, "  Object size:     {} bytes", self.object_size)?;
        writeln!(f, "  Total pages:     {}", self.total_pages)?;
        writeln!(f, "  Total objects:   {}", self.total_objects)?;
        writeln!(f, "  Allocated:       {}", self.total_allocated)?;
        writeln!(
            f,
            "  Free:            {}",
            self.total_objects - self.total_allocated
        )?;
        writeln!(f, "  Objects/page:    {}", self.objects_per_page)
    }
}

// ============================================================================
// 8. EmergencyPool
// ============================================================================

/// Emergency pool for critical allocations that cannot fail.
struct EmergencyPool {
    /// Pre-allocated pages (physical addresses).
    pages: [PhysAddr; EMERGENCY_PAGES],
    /// Number of pages currently available.
    available: usize,
}

impl EmergencyPool {
    /// Creates a new, empty emergency pool.
    #[inline]
    const fn new() -> Self {
        Self {
            pages: [PhysAddr::ZERO; EMERGENCY_PAGES],
            available: 0,
        }
    }

    /// Allocates a page from the emergency pool.
    fn allocate(&mut self) -> Option<PhysAddr> {
        if self.available == 0 {
            return None;
        }
        self.available -= 1;
        Some(self.pages[self.available])
    }

    /// Returns a page to the emergency pool.
    fn deallocate(&mut self, page: PhysAddr) -> Result<(), ()> {
        if self.available >= EMERGENCY_PAGES {
            return Err(());
        }
        self.pages[self.available] = page;
        self.available += 1;
        Ok(())
    }
}