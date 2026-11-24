//! Globalny alokator pamięci dla Rust core, używający C PMM.
use core::alloc::{GlobalAlloc, Layout};
use core::ptr;

extern "C" {
    fn pmm_alloc_page() -> *mut u8;
    fn pmm_free_page(page: *mut u8);
}

pub struct KernelAllocator;

unsafe impl GlobalAlloc for KernelAllocator {
    unsafe fn alloc(&self, layout: Layout) -> *mut u8 {
        // Dla uproszczenia alokujemy całe strony (4KB). 
        // W produkcyjnym systemie byłby tu allocator slab/buddy na tych stronach.
        let size = layout.size().max(layout.align()).max(4096);
        let _pages = (size + 4095) / 4096;
        
        let ptr = pmm_alloc_page();
        if ptr.is_null() {
            return ptr::null_mut();
        }
        
        // Wyzeruj alokowaną stronę dla bezpieczeństwa
        ptr::write_bytes(ptr, 0, 4096);
        ptr
    }

    unsafe fn dealloc(&self, ptr: *mut u8, _layout: Layout) {
        if !ptr.is_null() {
            pmm_free_page(ptr);
        }
    }
}

#[global_allocator]
static ALLOCATOR: KernelAllocator = KernelAllocator;
