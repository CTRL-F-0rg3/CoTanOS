// SPDX-FileCopyrightText: 2026 CTRL-F-0rg3
// SPDX-License-Identifier: GPL-3.0-only
//! CoTanOS 256xtr — kernel heap (frame-chunk allocator + `GlobalAlloc`).
//!
//! The kernel gets real `alloc` support (Vec, String, Box, ...) through a
//! minimal first-fit chunk allocator backed by whole physical frames from
//! `cotanos-mem`. Chunks are reused through a free list (no splitting, no
//! coalescing — fine for the small, bounded kernel allocations of 256xtr).

use core::alloc::{GlobalAlloc, Layout};
use core::ptr::NonNull;

use cotanos_mem::prelude::{PhysicalFrameAllocator, Size4Kb};

const FRAME: usize = 4096;
const HEADER: usize = 32;

#[repr(C)]
struct Chunk {
    size: usize, // whole chunk (header + payload) in bytes
    free: bool,
    next: *mut Chunk,
}

impl Chunk {
    fn payload(&self) -> *mut u8 {
        (self as *const Chunk as usize + HEADER) as *mut u8
    }
}

pub struct KernelHeap {
    head: *mut Chunk,
    allocator: &'static PhysicalFrameAllocator<Size4Kb>,
}

impl KernelHeap {
    pub fn new(allocator: &'static PhysicalFrameAllocator<Size4Kb>) -> Self {
        Self { head: core::ptr::null_mut(), allocator }
    }

    /// Grows the heap by one whole chunk of `frames` frames.
    unsafe fn grow(&mut self, frames: usize) -> Option<*mut Chunk> {
        let first = self.allocator.allocate_frame()?;
        let base = first.start_address().as_usize();
        for _ in 1..frames {
            let _ = self.allocator.allocate_frame()?;
        }
        unsafe {
            core::ptr::write_bytes(base as *mut u8, 0, frames * FRAME);
            let chunk = base as *mut Chunk;
            (*chunk).size = frames * FRAME;
            (*chunk).free = false;
            (*chunk).next = self.head;
            self.head = chunk;
        }
        Some(base as *mut Chunk)
    }

    /// Allocates `layout` — first fit over the free list, then grow.
    unsafe fn alloc(&mut self, layout: Layout) -> Option<NonNull<u8>> {
        let align = layout.align().max(16);
        let need = layout.size().max(8);

        // First fit.
        let mut prev: *mut Chunk = core::ptr::null_mut();
        let mut cur = self.head;
        while !cur.is_null() {
            let chunk = unsafe { &mut *cur };
            if chunk.free {
                let base = cur as usize;
                let p = (base + HEADER + align - 1) & !(align - 1);
                let total = (p - base) + need;
                if total <= chunk.size {
                    // Unlink and mark used.
                    let next = chunk.next;
                    if prev.is_null() {
                        self.head = next;
                    } else {
                        unsafe { (*prev).next = next };
                    }
                    unsafe {
                        (*chunk).size = total;
                        (*chunk).free = false;
                        (*chunk).next = next;
                    }
                    return NonNull::new(p as *mut u8);
                }
            }
            prev = cur;
            cur = chunk.next;
        }

        // Grow by whole frames.
        let frames = (HEADER + align + need).div_ceil(FRAME);
        let chunk = unsafe { self.grow(frames) }?;
        let base = chunk as usize;
        let p = (base + HEADER + align - 1) & !(align - 1);
        NonNull::new(p as *mut u8)
    }

    unsafe fn dealloc(&mut self, ptr: NonNull<u8>) {
        let addr = ptr.as_ptr() as usize;
        // Find the chunk containing `addr` and mark it free again.
        let mut cur = self.head;
        while !cur.is_null() {
            let chunk = unsafe { &mut *cur };
            let base = cur as usize;
            if !chunk.free && addr >= base && addr < base + chunk.size {
                unsafe {
                    (*chunk).free = true;
                }
                return;
            }
            cur = chunk.next;
        }
    }
}

static mut HEAP: Option<KernelHeap> = None;

/// Installs the global allocator heap over the frame allocator.
pub fn init(allocator: &'static PhysicalFrameAllocator<Size4Kb>) {
    unsafe {
        HEAP = Some(KernelHeap::new(allocator));
    }
}

struct GlobalAllocator;

unsafe impl GlobalAlloc for GlobalAllocator {
    unsafe fn alloc(&self, layout: Layout) -> *mut u8 {
        match HEAP.as_mut() {
            Some(h) => unsafe { h.alloc(layout) }.map(|p| p.as_ptr()).unwrap_or(core::ptr::null_mut()),
            None => core::ptr::null_mut(),
        }
    }

    unsafe fn dealloc(&self, ptr: *mut u8, _layout: Layout) {
        if let Some(h) = HEAP.as_mut() {
            unsafe { h.dealloc(NonNull::new_unchecked(ptr)) };
        }
    }
}

#[global_allocator]
static GLOBAL_ALLOCATOR: GlobalAllocator = GlobalAllocator;
