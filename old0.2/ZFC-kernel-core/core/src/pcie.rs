//! PCIe enumeration module.
//! Scans the PCIe bus to discover devices and read their configuration.

use cotan_kapi::kprintln;
use crate::memory::map_mmio_region;

/// PCIe Configuration Space offsets
const PCI_VENDOR_ID: usize = 0x00;
const PCI_DEVICE_ID: usize = 0x02;
const PCI_CLASS_CODE: usize = 0x09;
const PCI_SUBCLASS: usize = 0x0A;
const PCI_BAR0: usize = 0x10;

/// Invalid vendor ID (indicates no device present)
const PCI_INVALID_VENDOR: u16 = 0xFFFF;

/// PCIe ECAM size (256 MB for bus 0-255)
const PCIE_ECAM_SIZE: usize = 256 * 1024 * 1024;

/// Represents a discovered PCIe device
pub struct PcieDevice {
    pub bus: u8,
    pub device: u8,
    pub function: u8,
    pub vendor_id: u16,
    pub device_id: u16,
    pub class_code: u8,
    pub subclass: u8,
    pub bar0: u32,
}

impl PcieDevice {
    /// Reads configuration from PCIe MMIO space
    pub fn read_config(ecam_base: usize, bus: u8, device: u8, function: u8, offset: usize) -> u32 {
        let addr = ecam_base 
            + ((bus as usize) << 20) 
            + ((device as usize) << 15) 
            + ((function as usize) << 12) 
            + offset;
        
        unsafe { core::ptr::read_volatile(addr as *const u32) }
    }

    /// Scans a single device/function
    pub fn scan_device(ecam_base: usize, bus: u8, device: u8, function: u8) -> Option<PcieDevice> {
        let vendor_id = Self::read_config(ecam_base, bus, device, function, PCI_VENDOR_ID) as u16;
        
        if vendor_id == PCI_INVALID_VENDOR {
            return None;
        }

        let device_id = (Self::read_config(ecam_base, bus, device, function, PCI_DEVICE_ID) >> 16) as u16;
        let class_reg = Self::read_config(ecam_base, bus, device, function, PCI_CLASS_CODE);
        let class_code = ((class_reg >> 8) & 0xFF) as u8;
        let subclass = ((class_reg >> 16) & 0xFF) as u8;
        let bar0 = Self::read_config(ecam_base, bus, device, function, PCI_BAR0);

        Some(PcieDevice {
            bus,
            device,
            function,
            vendor_id,
            device_id,
            class_code,
            subclass,
            bar0,
        })
    }

    /// Returns a human-readable class description
    pub fn class_name(&self) -> &'static str {
        match self.class_code {
            0x01 => "Mass Storage",
            0x02 => "Network",
            0x03 => "Display",
            0x04 => "Multimedia",
            0x05 => "Memory",
            0x06 => "Bridge",
            0x07 => "Communication",
            0x08 => "System Peripheral",
            0x09 => "Input Device",
            0x0A => "Docking Station",
            0x0B => "Processor",
            0x0C => "Serial Bus",
            0x0D => "Wireless",
            _ => "Unknown",
        }
    }
}

/// Scans the entire PCIe bus and returns all discovered devices
pub fn enumerate_pcie(ecam_base: usize) -> alloc::vec::Vec<PcieDevice> {
    use alloc::vec::Vec;
    let mut devices = Vec::new();

    kprintln!("[pcie] mapping ECAM region @ {:#x}...", ecam_base);
    let mapped_ecam = map_mmio_region(ecam_base, PCIE_ECAM_SIZE);
    
    if mapped_ecam == 0 {
        kprintln!("[pcie] failed to map ECAM region");
        return devices;
    }

    kprintln!("[pcie] scanning PCIe bus @ {:#x}...", mapped_ecam);

    for device in 0..32u8 {
        for function in 0..8u8 {
            if let Some(dev) = PcieDevice::scan_device(mapped_ecam, 0, device, function) {
                kprintln!("[pcie] found device: {:02x}:{:02x}.{} - Vendor={:#06x}, Device={:#06x}, Class={:02x}:{:02x} ({})",
                    dev.bus, dev.device, dev.function,
                    dev.vendor_id, dev.device_id,
                    dev.class_code, dev.subclass,
                    dev.class_name());
                devices.push(dev);
            }
        }
    }

    kprintln!("[pcie] scan complete. Found {} devices.", devices.len());
    devices
}
