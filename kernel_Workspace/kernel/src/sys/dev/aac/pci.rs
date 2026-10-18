// pci.rs

use crate::sys::dev::aac::tables::*;

use crate::pci::{config_read_u16, config_read_u32, config_write_u32};

// Re-export PciError-like type for compatibility
#[derive(Debug)]
pub enum PciError {
    DeviceNotSupported,
    BusMasterFailed,
}

// VirtAddr / PhysAddr compatibility shim
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub struct VirtAddr(pub u64);
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub struct PhysAddr(pub u64);

impl VirtAddr {
    pub fn as_u64(&self) -> u64 { self.0 }
}

/// Minimal PCI device descriptor for the AAC driver.
pub struct PciDev {
    pub bus: u8,
    pub dev: u8,
    pub func: u8,
    pub vendor_id: u16,
    pub device_id: u16,
    pub subvendor_id: u16,
    pub subdevice_id: u16,
}

impl PciDev {
    fn read_config_u32(&self, offset: u8) -> u32 {
        let addr = crate::pci::PciAddress {
            bus: self.bus,
            device: self.dev,
            function: self.func,
        };
        config_read_u32(addr, offset)
    }

    fn read_config_u16(&self, offset: u8) -> u16 {
        let addr = crate::pci::PciAddress {
            bus: self.bus,
            device: self.dev,
            function: self.func,
        };
        crate::pci::config_read_u16(addr, offset)
    }

    pub fn write_config_u16(&self, offset: u8, value: u16) {
        let addr = crate::pci::PciAddress {
            bus: self.bus,
            device: self.dev,
            function: self.func,
        };
        let base = offset & 0xFC;
        let shift = ((offset & 2) as u32) * 8;
        let mut current = crate::pci::config_read_u32(addr, base);
        current &= !(0xFFFFu32 << shift);
        current |= (value as u32) << shift;
        crate::pci::config_write_u32(addr, base, current);
    }

    fn write_config_u32(&self, offset: u8, value: u32) {
        let addr = crate::pci::PciAddress {
            bus: self.bus,
            device: self.dev,
            function: self.func,
        };
        config_write_u32(addr, offset, value)
    }

    pub fn enable_busmaster(&self) -> Result<(), PciError> {
        let cmd = self.read_config_u16(0x04);
        if cmd & 0x04u16 == 0 {
            self.write_config_u16(0x04, cmd | 0x04u16);
        }
        let cmd = self.read_config_u16(0x04);
        if cmd & 0x04u16 == 0 {
            return Err(PciError::BusMasterFailed);
        }
        Ok(())
    }
}

pub struct AacSoftc {
    pub pci_dev: PciDev,
    pub hwif: u32,
    pub quirks: u32,
    pub regs_bar0: VirtAddr,
    pub regs_bar1: Option<VirtAddr>,
    pub irq_vector: u32,
    pub is_64bit: bool,
    // Tu będą później: kolejki FIB, DMA tagi, lista dysków
}

impl AacSoftc {
    pub fn probe(bus: u8, dev: u8, func: u8) -> Option<&'static str> {
        let vendor_id = config_read_u16(
            crate::pci::PciAddress { bus, device: dev, function: func }, 0x00);
        let device_id = config_read_u16(
            crate::pci::PciAddress { bus, device: dev, function: func }, 0x02);
        let subvendor_id = config_read_u16(
            crate::pci::PciAddress { bus, device: dev, function: func }, 0x2C);
        let subdevice_id = config_read_u16(
            crate::pci::PciAddress { bus, device: dev, function: func }, 0x2E);

        let id = find_pci_id(vendor_id, device_id, subvendor_id, subdevice_id)?;
        Some(id.desc)
    }

    pub fn attach(pci_dev: PciDev) -> Result<Self, PciError> {
        let vendor_id = pci_dev.vendor_id;
        let device_id = pci_dev.device_id;
        let subvendor_id = pci_dev.subvendor_id;
        let subdevice_id = pci_dev.subdevice_id;

        let id = find_pci_id(vendor_id, device_id, subvendor_id, subdevice_id)
            .ok_or(PciError::DeviceNotSupported)?;

        pci_dev.enable_busmaster()?;

        let cmd = pci_dev.read_config_u16(0x04);
        if cmd & 0x04 == 0 {
            return Err(PciError::BusMasterFailed);
        }

        // BAR0 gives us the register window (32-bit BAR, low 32 bits).
        let bar0 = pci_dev.read_config_u32(0x10) & !0xF;
        let regs_bar0 = VirtAddr(bar0 as u64);

        let regs_bar1 = if id.hwif == AAC_HWIF_NARK {
            let bar1 = pci_dev.read_config_u32(0x14) & !0xF;
            Some(VirtAddr(bar1 as u64))
        } else {
            None
        };

        // Legacy IRQ - for now we just use a placeholder vector.
        let irq_vector = 0;

        Ok(AacSoftc {
            pci_dev,
            hwif: id.hwif,
            quirks: id.quirks,
            regs_bar0,
            regs_bar1,
            irq_vector,
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