//! Direct Memory Access (DMA) allocation abstraction.

use cotan_kapi::kprintln;

/// Represents an error during DMA operations.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum DmaError {
    OutOfMemory,
    InvalidAlignment,
    NotSupported,
}

/// Trait for allocating DMA-coherent memory.
pub trait DmaAllocator {
    /// Allocates a contiguous block of memory suitable for DMA.
    /// Returns the physical address and a virtual pointer.
    fn alloc(&self, size: usize, align: usize) -> Result<(usize, *mut u8), DmaError>;
    
    /// Frees a previously allocated DMA block.
    fn free(&self, phys_addr: usize, virt_addr: *mut u8, size: usize);
}

/// A simple identity-mapped DMA allocator.
/// It assumes virtual address == physical address.
pub struct IdentityDmaAllocator;

impl IdentityDmaAllocator {
    pub const fn new() -> Self {
        Self
    }
}

impl DmaAllocator for IdentityDmaAllocator {
    fn alloc(&self, size: usize, align: usize) -> Result<(usize, *mut u8), DmaError> {
        if align > 4096 {
            return Err(DmaError::InvalidAlignment);
        }
        
        // TODO: Integrate with actual PMM (e.g., pmm_alloc_pages)
        // For now, we simulate allocation with a safe dummy physical address.
        kprintln!("[dma] IdentityDmaAllocator: allocated {} bytes (simulated)", size);
        
        let phys_addr = 0x81000000usize; 
        let virt_addr = phys_addr as *mut u8;
        
        Ok((phys_addr, virt_addr))
    }

    fn free(&self, _phys_addr: usize, _virt_addr: *mut u8, _size: usize) {
        kprintln!("[dma] IdentityDmaAllocator: freed memory (simulated)");
    }
}
