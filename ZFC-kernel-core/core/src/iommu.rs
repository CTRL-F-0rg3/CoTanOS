//! IOMMU (Input-Output Memory Management Unit) abstraction.
//! Provides address translation and isolation for DMA-capable devices.

use cotan_kapi::kprintln;

/// Represents an error during IOMMU operations.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum IommuError {
    InvalidAddress,
    MappingFailed,
    NotSupported,
}

/// Trait defining IOMMU operations.
pub trait IommuDevice {
    /// Maps a device-visible IOVA (IO Virtual Address) to a physical address.
    fn map(&mut self, iova: usize, phys_addr: usize, size: usize, flags: u32) -> Result<(), IommuError>;
    
    /// Unmaps a previously mapped IOVA range.
    fn unmap(&mut self, iova: usize, size: usize) -> Result<(), IommuError>;
    
    /// Enables the IOMMU for a specific device or domain.
    fn enable(&mut self) -> Result<(), IommuError>;
}

/// A stub IOMMU implementation that performs identity mapping.
/// This is the baseline before hardware-specific register programming is added.
pub struct StubIommu {
    enabled: bool,
}

impl StubIommu {
    pub const fn new() -> Self {
        Self { enabled: false }
    }
}

impl IommuDevice for StubIommu {
    fn map(&mut self, iova: usize, phys_addr: usize, size: usize, _flags: u32) -> Result<(), IommuError> {
        if iova != phys_addr {
            kprintln!("[iommu] StubIommu: non-identity mapping requested (IOVA: {:#x}, Phys: {:#x})", iova, phys_addr);
            // In a full driver, this would walk the IOMMU page tables (e.g., Sv39x4)
            // and write PTEs to the IOMMU memory.
        }
        kprintln!("[iommu] StubIommu: mapped {:#x} -> {:#x} (size: {:#x})", iova, phys_addr, size);
        Ok(())
    }

    fn unmap(&mut self, iova: usize, size: usize) -> Result<(), IommuError> {
        kprintln!("[iommu] StubIommu: unmapped {:#x} (size: {:#x})", iova, size);
        Ok(())
    }

    fn enable(&mut self) -> Result<(), IommuError> {
        self.enabled = true;
        kprintln!("[iommu] StubIommu: enabled (identity mapping mode)");
        Ok(())
    }
}
