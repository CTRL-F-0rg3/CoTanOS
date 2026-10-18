// pci.rs

use core::ptr::NonNull;
use crate::drivers::aac::tables::*;
use crate::kernel::pci::{PciDevice, PciError};
use crate::kernel::mm::{VirtAddr, PhysAddr};

pub struct AacSoftc {
    pub pci_dev: NonNull<dyn PciDevice>,
    pub hwif: u32,
    pub quirks: u32,
    pub regs_bar0: VirtAddr,
    pub regs_bar1: Option<VirtAddr>,
    pub irq_vector: u32,
    pub is_64bit: bool,
    // Tu będą później: kolejki FIB, DMA tagi, lista dysków
}

impl AacSoftc {
    pub fn probe(pci: &dyn PciDevice) -> Option<&'static str> {
        let id = find_pci_id(
            pci.vendor_id(), 
            pci.device_id(), 
            pci.subvendor_id(), 
            pci.subdevice_id()
        )?;
        Some(id.desc)
    }

    pub fn attach(pci: &mut dyn PciDevice) -> Result<Self, PciError> {
        let id = find_pci_id(
            pci.vendor_id(), 
            pci.device_id(), 
            pci.subvendor_id(), 
            pci.subdevice_id()
        ).ok_or(PciError::DeviceNotSupported)?;

        pci.enable_busmaster()?;
        
        let cmd = pci.read_config(0x04, 2);
        if cmd & 0x04 == 0 {
            return Err(PciError::BusMasterFailed);
        }

        let regs_bar0 = pci.map_bar(0)?;
        let regs_bar1 = if id.hwif == AAC_HWIF_NARK {
            Some(pci.map_bar(1)?)
        } else {
            None
        };

        let irq = pci.allocate_interrupt()?;

        Ok(AacSoftc {
            pci_dev: NonNull::new(pci as *mut dyn PciDevice).unwrap(),
            hwif: id.hwif,
            quirks: id.quirks,
            regs_bar0,
            regs_bar1,
            irq_vector: irq,
            is_64bit: (id.quirks & AAC_FLAGS_NO4GB) == 0,
        })
    }

    pub fn read_reg(&self, offset: u32) -> u32 {
        unsafe { core::ptr::read_volatile((self.regs_bar0.as_u64() + offset as u64) as *const u32) }
    }

    pub fn write_reg(&self, offset: u32, value: u32) {
        unsafe { core::ptr::write_volatile((self.regs_bar0.as_u64() + offset as u64) as *mut u32, value) }
    }
}