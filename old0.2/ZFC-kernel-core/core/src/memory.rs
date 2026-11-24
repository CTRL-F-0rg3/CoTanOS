//! Zarządzanie pamięcią wirtualną
//! Współpracuje z C PMM z init

use spin::Mutex;
use bitflags::bitflags;

bitflags! {
    /// Flagi stron pamięci
    pub struct PageFlags: u64 {
        const PRESENT = 1 << 0;
        const WRITABLE = 1 << 1;
        const USER = 1 << 2;
        const EXECUTABLE = 1 << 3;
    }
}

/// Menedżer pamięci wirtualnej
pub struct VMM {
    // TODO: Struktury page table
}

impl VMM {
    pub const fn new() -> Self {
        Self {}
    }

    /// Mapuj stronę fizyczną do wirtualnej
    pub fn map_page(&mut self, _vaddr: usize, _paddr: usize, _flags: PageFlags) -> Result<(), &'static str> {
        // TODO: Implementacja
        Ok(())
    }

    /// Odmapuj stronę
    pub fn unmap_page(&mut self, _vaddr: usize) -> Result<(), &'static str> {
        // TODO: Implementacja
        Ok(())
    }
}

/// Globalna instancja VMM
pub static VMM: Mutex<VMM> = Mutex::new(VMM::new());

/// Maps a physical MMIO region into the kernel's virtual address space.
/// This is needed for devices like PCIe ECAM that are not part of the kernel image.
/// Returns the virtual address where the region is mapped.
pub fn map_mmio_region(phys_addr: usize, size: usize) -> usize {
    cotan_kapi::kprintln!("[memory] mapping MMIO region: phys={:#x}, size={:#x}", phys_addr, size);
    
    // Call into the C MM code to do the actual mapping
    extern "C" {
        fn mm_map_mmio_region(phys: u64, size: u64) -> u64;
    }
    
    let vaddr = unsafe { mm_map_mmio_region(phys_addr as u64, size as u64) };
    cotan_kapi::kprintln!("[memory] MMIO region mapped at vaddr={:#x}", vaddr);
    vaddr as usize
}
