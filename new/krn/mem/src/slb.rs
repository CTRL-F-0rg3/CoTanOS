// SPDX-FileCopyrightText: 2026 CTRL-F-0rg3
// SPDX-License-Identifier: GPL-3.0-only
//! CoTanOS Kernel Slab Allocator (`slb.rs`)

use core::fmt;
use core::ptr::NonNull;

use crate::frm::{FrameError, IrqSpinlock, PhysAddr, PhysFrame, Size4Kb};

// ============================================================================
// Constants & Debug Patterns
// ============================================================================

#[cfg(debug_assertions)]
const POISON_PATTERN: u8 = 0xDE;

#[cfg(debug_assertions)]
const REDZONE_PATTERN: u8 = 0xAD;

#[cfg(debug_assertions)]
const REDZONE_SIZE: usize = 16;

const MIN_ALIGNMENT: usize = 8;

// ============================================================================
// 1. SlabError
// ============================================================================

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum SlabError {
    SizeTooLarge { requested: usize, max_supported: usize },
    OutOfMemory,
    InvalidObject { addr: usize, expected_size: usize },
    CacheNotInitialized,
    FrameAllocatorError(FrameError),
    #[cfg(debug_assertions)]
    RedzoneCorruption { addr: usize, location: &'static str },
}

impl fmt::Display for SlabError {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            Self::SizeTooLarge { requested, max_supported } => {
                write!(f, "slab: requested size {} exceeds max supported {}", requested, max_supported)
            }
            Self::OutOfMemory => write!(f, "slab: out of memory"),
            Self::InvalidObject { addr, expected_size } => {
                write!(f, "slab: invalid object at {:#x} (expected size {})", addr, expected_size)
            }
            Self::CacheNotInitialized => write!(f, "slab: cache not initialized"),
            Self::FrameAllocatorError(e) => write!(f, "slab: frame allocator error: {}", e),
            #[cfg(debug_assertions)]
            Self::RedzoneCorruption { addr, location } => {
                write!(f, "slab: redzone corruption at {:#x} ({})", addr, location)
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
// 2. SlabSize
// ============================================================================

#[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord)]
#[repr(usize)]
pub enum SlabSize {
    Size32 = 32, Size64 = 64, Size96 = 96, Size128 = 128, Size192 = 192,
    Size256 = 256, Size512 = 512, Size1024 = 1024, Size2048 = 2048,
    Size4096 = 4096, Size8192 = 8192,
}

impl SlabSize {
    #[inline]
    pub const fn size(self) -> usize { self as usize }

    #[inline]
    pub const fn name(self) -> &'static str {
        match self {
            Self::Size32 => "slab-32", Self::Size64 => "slab-64", Self::Size96 => "slab-96",
            Self::Size128 => "slab-128", Self::Size192 => "slab-192", Self::Size256 => "slab-256",
            Self::Size512 => "slab-512", Self::Size1024 => "slab-1024", Self::Size2048 => "slab-2048",
            Self::Size4096 => "slab-4096", Self::Size8192 => "slab-8192",
        }
    }

    #[inline]
    pub fn for_size(requested: usize) -> Option<Self> {
        if requested == 0 { return Some(Self::Size32); }
        if requested <= 32 { Some(Self::Size32) }
        else if requested <= 64 { Some(Self::Size64) }
        else if requested <= 96 { Some(Self::Size96) }
        else if requested <= 128 { Some(Self::Size128) }
        else if requested <= 192 { Some(Self::Size192) }
        else if requested <= 256 { Some(Self::Size256) }
        else if requested <= 512 { Some(Self::Size512) }
        else if requested <= 1024 { Some(Self::Size1024) }
        else if requested <= 2048 { Some(Self::Size2048) }
        else if requested <= 4096 { Some(Self::Size4096) }
        else if requested <= 8192 { Some(Self::Size8192) }
        else { None }
    }
}

// ============================================================================
// 3. SlabPageHeader
// ============================================================================

#[repr(C)]
struct SlabPageHeader {
    magic: u32,
    inuse: u16,
    total: u16,
    free_head: u16,
    next: PhysAddr,
    prev: PhysAddr,
}

const SLAB_PAGE_MAGIC: u32 = 0x534C4142; // "SLAB"

impl SlabPageHeader {
    #[inline]
    const fn new() -> Self {
        Self { magic: SLAB_PAGE_MAGIC, inuse: 0, total: 0, free_head: u16::MAX, next: PhysAddr::ZERO, prev: PhysAddr::ZERO }
    }
    #[inline]
    const fn is_empty(&self) -> bool { self.inuse == 0 }
    #[inline]
    const fn is_full(&self) -> bool { self.inuse == self.total }
}

// ============================================================================
// 4. SlabCache
// ============================================================================

struct SlabCache {
    name: &'static str,
    object_size: usize,
    aligned_size: usize,
    objects_per_page: usize,
    partial_pages: PhysAddr,
    full_pages: PhysAddr,
    empty_pages: PhysAddr,
    total_pages: usize,
    total_allocated: usize,
    ctor: Option<fn(*mut u8)>,
    dtor: Option<fn(*mut u8)>,
}

impl SlabCache {
    #[inline]
    unsafe fn new(name: &'static str, object_size: usize) -> Self {
        let aligned_size = Self::align_size(object_size);
        let header_size = core::mem::size_of::<SlabPageHeader>();
        let usable_space = Size4Kb::SIZE - header_size;
        let objects_per_page = usable_space / aligned_size;

        Self {
            name, object_size, aligned_size, objects_per_page,
            partial_pages: PhysAddr::ZERO, full_pages: PhysAddr::ZERO, empty_pages: PhysAddr::ZERO,
            total_pages: 0, total_allocated: 0, ctor: None, dtor: None,
        }
    }

    #[inline]
    fn align_size(size: usize) -> usize {
        let mut aligned = (size + MIN_ALIGNMENT - 1) & !(MIN_ALIGNMENT - 1);
        #[cfg(debug_assertions)]
        { aligned += REDZONE_SIZE * 2; }
        aligned
    }

    #[inline]
    fn set_ctor(&mut self, ctor: fn(*mut u8)) { self.ctor = Some(ctor); }

    #[inline]
    fn set_dtor(&mut self, dtor: fn(*mut u8)) { self.dtor = Some(dtor); }

    fn grow<F: FnMut(usize) -> Result<PhysFrame<Size4Kb>, FrameError>>(
        &mut self,
        alloc_frame: &mut F,
    ) -> Result<(), SlabError> {
        let frame = alloc_frame(self.object_size)?;
        let page_addr = frame.start_address();

        unsafe {
            let header = &mut *(page_addr.as_usize() as *mut SlabPageHeader);
            *header = SlabPageHeader::new();
            header.total = self.objects_per_page as u16;

            let mut free_head = u16::MAX;
            for i in (0..self.objects_per_page).rev() {
                let obj_addr = Self::object_address(page_addr, i, self.aligned_size);
                let next_ptr = obj_addr.as_usize() as *mut u16;
                *next_ptr = free_head;
                free_head = i as u16;

                #[cfg(debug_assertions)]
                {
                    let obj_ptr = obj_addr.as_usize() as *mut u8;
                    core::ptr::write_bytes(obj_ptr, POISON_PATTERN, self.aligned_size);
                }
            }
            header.free_head = free_head;
            Self::push_page(&mut self.empty_pages, page_addr);
        }

        self.total_pages += 1;
        Ok(())
    }

    #[inline]
    fn object_address(page_addr: PhysAddr, index: usize, aligned_size: usize) -> PhysAddr {
        let offset = core::mem::size_of::<SlabPageHeader>() + index * aligned_size;
        PhysAddr::new(page_addr.as_usize() + offset)
    }

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

    fn allocate<F: FnMut(usize) -> Result<PhysFrame<Size4Kb>, FrameError>>(
        &mut self,
        alloc_frame: &mut F,
    ) -> Result<NonNull<u8>, SlabError> {
        if !self.partial_pages.is_null() {
            unsafe { self.allocate_from_list(true, alloc_frame) }
        } else if !self.empty_pages.is_null() {
            unsafe { self.allocate_from_list(false, alloc_frame) }
        } else {
            self.grow(alloc_frame)?;
            unsafe { self.allocate_from_list(false, alloc_frame) }
        }
    }

    unsafe fn allocate_from_list<F: FnMut(usize) -> Result<PhysFrame<Size4Kb>, FrameError>>(
        &mut self,
        is_partial: bool,
        _alloc_frame: &mut F,
    ) -> Result<NonNull<u8>, SlabError> {
        let list = if is_partial { &mut self.partial_pages } else { &mut self.empty_pages };
        let page_addr = *list;
        let header = &mut *(page_addr.as_usize() as *mut SlabPageHeader);

        debug_assert!(header.magic == SLAB_PAGE_MAGIC);
        debug_assert!(header.free_head != u16::MAX);

        let obj_index = header.free_head as usize;
        let obj_addr = Self::object_address(page_addr, obj_index, self.aligned_size);

        let next_ptr = obj_addr.as_usize() as *const u16;
        header.free_head = unsafe { *next_ptr };
        header.inuse += 1;

        if is_partial {
            if header.is_full() {
                Self::remove_page(list, page_addr);
                Self::push_page(&mut self.full_pages, page_addr);
            }
        } else {
            Self::remove_page(list, page_addr);
            Self::push_page(&mut self.partial_pages, page_addr);
        }

        self.total_allocated += 1;

        #[cfg(debug_assertions)]
        {
            let obj_ptr = obj_addr.as_usize() as *mut u8;
            unsafe {
                core::ptr::write_bytes(obj_ptr, REDZONE_PATTERN, REDZONE_SIZE);
                core::ptr::write_bytes(obj_ptr.add(REDZONE_SIZE + self.object_size), REDZONE_PATTERN, REDZONE_SIZE);
            }
        }

        if let Some(ctor) = self.ctor {
            let user_ptr = self.user_pointer(obj_addr);
            unsafe { ctor(user_ptr.as_ptr() as *mut u8); }
        }

        Ok(self.user_pointer(obj_addr))
    }

    #[inline]
    fn user_pointer(&self, obj_addr: PhysAddr) -> NonNull<u8> {
        #[cfg(debug_assertions)]
        { NonNull::new((obj_addr.as_usize() + REDZONE_SIZE) as *mut u8).unwrap() }
        #[cfg(not(debug_assertions))]
        { NonNull::new(obj_addr.as_usize() as *mut u8).unwrap() }
    }

    fn deallocate<F: FnMut(PhysFrame<Size4Kb>) -> Result<(), FrameError>>(
        &mut self,
        obj_ptr: NonNull<u8>,
        _free_frame: &mut F,
    ) -> Result<(), SlabError> {
        let obj_addr = self.object_pointer(obj_ptr);
        let page_addr = self.page_for_object(obj_addr);

        unsafe {
            let header = &mut *(page_addr.as_usize() as *mut SlabPageHeader);

            if header.magic != SLAB_PAGE_MAGIC {
                return Err(SlabError::InvalidObject { addr: obj_ptr.as_ptr() as usize, expected_size: self.object_size });
            }

            #[cfg(debug_assertions)]
            {
                let obj_raw = obj_addr.as_usize() as *mut u8;
                unsafe {
                    for i in 0..REDZONE_SIZE {
                        if *obj_raw.add(i) != REDZONE_PATTERN {
                            return Err(SlabError::RedzoneCorruption { addr: obj_ptr.as_ptr() as usize, location: "before object" });
                        }
                    }
                    for i in 0..REDZONE_SIZE {
                        if *obj_raw.add(REDZONE_SIZE + self.object_size + i) != REDZONE_PATTERN {
                            return Err(SlabError::RedzoneCorruption { addr: obj_ptr.as_ptr() as usize, location: "after object" });
                        }
                    }
                    core::ptr::write_bytes(obj_raw, POISON_PATTERN, self.aligned_size);
                }
            }

            if let Some(dtor) = self.dtor {
                dtor(obj_ptr.as_ptr());
            }

            let obj_index = self.object_index(obj_addr, page_addr);
            let next_ptr = obj_addr.as_usize() as *mut u16;
            *next_ptr = header.free_head;
            header.free_head = obj_index as u16;
            header.inuse -= 1;

            if header.is_full() {
                Self::remove_page(&mut self.full_pages, page_addr);
                Self::push_page(&mut self.partial_pages, page_addr);
            } else if header.is_empty() {
                Self::remove_page(&mut self.partial_pages, page_addr);
                Self::push_page(&mut self.empty_pages, page_addr);
            }

            self.total_allocated -= 1;
        }
        Ok(())
    }

    #[inline]
    fn object_pointer(&self, user_ptr: NonNull<u8>) -> PhysAddr {
        #[cfg(debug_assertions)]
        { PhysAddr::new(user_ptr.as_ptr() as usize - REDZONE_SIZE) }
        #[cfg(not(debug_assertions))]
        { PhysAddr::new(user_ptr.as_ptr() as usize) }
    }

    #[inline]
    fn page_for_object(&self, obj_addr: PhysAddr) -> PhysAddr {
        PhysAddr::new(obj_addr.as_usize() & !(Size4Kb::SIZE - 1))
    }

    #[inline]
    fn object_index(&self, obj_addr: PhysAddr, page_addr: PhysAddr) -> usize {
        let offset = obj_addr.as_usize() - page_addr.as_usize() - core::mem::size_of::<SlabPageHeader>();
        offset / self.aligned_size
    }

    fn stats(&self) -> SlabCacheStats {
        SlabCacheStats {
            name: self.name, object_size: self.object_size, total_pages: self.total_pages,
            total_allocated: self.total_allocated, total_objects: self.total_pages * self.objects_per_page,
            objects_per_page: self.objects_per_page,
        }
    }
}

// ============================================================================
// 5. SlabAllocator
// ============================================================================

pub struct SlabAllocator {
    caches: [IrqSpinlock<SlabCache>; 11],
    frame_allocator: &'static crate::frm::PhysicalFrameAllocator<Size4Kb>,
}

impl SlabAllocator {
    pub unsafe fn new(frame_allocator: &'static crate::frm::PhysicalFrameAllocator<Size4Kb>) -> Self {
        let caches = unsafe { Self::init_caches() };
        Self { caches, frame_allocator }
    }

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

    pub fn allocate(&self, size: usize) -> Result<NonNull<u8>, SlabError> {
        let slab_size = SlabSize::for_size(size).ok_or(SlabError::SizeTooLarge { requested: size, max_supported: 8192 })?;
        let cache_index = slab_size as usize;
        let mut cache = self.caches[cache_index].lock();
        let mut alloc_frame = |_: usize| -> Result<PhysFrame<Size4Kb>, FrameError> {
            self.frame_allocator.allocate_frame().ok_or(FrameError::OutOfMemory)
        };
        cache.allocate(&mut alloc_frame)
    }

    pub unsafe fn deallocate(&self, ptr: NonNull<u8>, size: usize) -> Result<(), SlabError> {
        let slab_size = SlabSize::for_size(size).ok_or(SlabError::SizeTooLarge { requested: size, max_supported: 8192 })?;
        let cache_index = slab_size as usize;
        let mut cache = self.caches[cache_index].lock();
        let mut free_frame = |_: PhysFrame<Size4Kb>| -> Result<(), FrameError> { Ok(()) };
        cache.deallocate(ptr, &mut free_frame)
    }

    pub fn stats(&self) -> [Option<SlabCacheStats>; 11] {
        let mut result: [Option<SlabCacheStats>; 11] = [None; 11];
        for (i, cache) in self.caches.iter().enumerate() {
            result[i] = Some(cache.lock().stats());
        }
        result
    }
}

// ============================================================================
// 6. SlabCacheStats
// ============================================================================

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
        writeln!(f, "  Free:            {}", self.total_objects - self.total_allocated)?;
        writeln!(f, "  Objects/page:    {}", self.objects_per_page)
    }
}
