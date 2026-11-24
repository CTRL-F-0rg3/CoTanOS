use cotan_kapi::ffi::{pmm_alloc_page, pmm_free_page};
use virtio_drivers::{Hal, BufferDirection};
use core::ptr::NonNull;

pub struct CotanHal;

unsafe impl Hal for CotanHal {
    fn dma_alloc(pages: usize, _direction: BufferDirection) -> (u64, NonNull<u8>) {
        let mut addrs = alloc::vec::Vec::with_capacity(pages);
        for _ in 0..pages {
            let addr = unsafe { pmm_alloc_page() } as usize;
            if addr == 0 {
                for a in addrs { unsafe { pmm_free_page(a as *mut u8) }; }
                panic!("DMA alloc failed");
            }
            addrs.push(addr);
        }
        let vaddr = addrs[0] as *mut u8;
        let paddr = addrs[0] as u64;
        (paddr, NonNull::new(vaddr).unwrap())
    }

    unsafe fn dma_dealloc(paddr: u64, _vaddr: NonNull<u8>, pages: usize) -> i32 {
        let paddr = paddr as usize;
        for i in 0..pages {
            pmm_free_page((paddr + i * 4096) as *mut u8);
        }
        0
    }

    unsafe fn mmio_phys_to_virt(paddr: u64, _size: usize) -> NonNull<u8> {
        NonNull::new(paddr as *mut u8).unwrap()
    }

    unsafe fn share(buffer: NonNull<[u8]>, _direction: BufferDirection) -> u64 {
        // Identity mapping: virtual address is the physical address
        buffer.as_ptr() as *mut u8 as u64
    }

    unsafe fn unshare(_paddr: u64, _buffer: NonNull<[u8]>, _direction: BufferDirection) {
        // Nothing to do for identity mapping
    }
}
