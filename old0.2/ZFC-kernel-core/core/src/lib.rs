#![no_std]
extern crate alloc;

pub mod kalloc;
pub mod kpanic;
pub mod ax_stubs;
pub mod memory;
pub mod process;
pub mod syscall;
pub mod power;
pub mod thermal;
pub mod pcie;
pub mod peripherals;
pub mod dma;
pub mod iommu;
pub mod rtc;

use cotan_kapi::kprintln;
use fdt::Fdt;
use peripherals::{GpioPin, I2cDevice, SpiDevice};
use peripherals::gpio::StubGpioPin;
use peripherals::i2c::StubI2cDevice;
use peripherals::spi::StubSpiDevice;
use dma::{DmaAllocator, IdentityDmaAllocator};
use iommu::{IommuDevice, StubIommu};
use rtc::{Rtc, Pcf8563Rtc, RtcTime};

#[no_mangle]
pub extern "C" fn core_init(dtb: u64) -> i32 {
    cotan_kapi::logging::init();
    kprintln!("[rust] CoTanOS core up");

    let probe: alloc::vec::Vec<u32> = alloc::vec![1, 2, 3];
    kprintln!("[rust] kalloc probe: {} elems @ {:p}", probe.len(), probe.as_ptr());
    drop(probe);

    // 1. Parse Device Tree
    kprintln!("[rust] parsing device tree @ {:#x}", dtb);
    let fdt_ptr = dtb as *const u8;
    let fdt = unsafe { 
        match Fdt::from_ptr(fdt_ptr) {
            Ok(f) => f,
            Err(e) => {
                kprintln!("[rust] FDT parse error: {:?}", e);
                return -1;
            }
        }
    };

    let mut net_found = false;
    let mut blk_found = false;
    let mut ecam_base: Option<usize> = None;

    // 2. Initialize devices from DTB
    for node in fdt.all_nodes() {
        if let Some(compat) = node.compatible() {
            let is_virtio = compat.all().any(|c| c == "virtio,mmio");
            let is_pcie_ecam = compat.all().any(|c| c.contains("pci-host-ecam-generic") || c.contains("pci-host-cam-generic"));
            
            if is_virtio {
                if let Some(reg) = node.reg().and_then(|mut r| r.next()) {
                    let addr = reg.starting_address as usize;
                    kprintln!("[rust] found virtio-mmio @ {:#x}", addr);

                    if !net_found {
                        if cotan_net::init_network(addr).is_ok() {
                            kprintln!("[rust] net: initialized successfully");
                            net_found = true;
                            continue;
                        }
                    }
                    
                    if !blk_found {
                        kprintln!("[rust] storage: probing addr {:#x} for blk...", addr);
                        match cotan_storage::init_storage(addr) {
                            Ok(_) => {
                                kprintln!("[rust] storage: initialized successfully");
                                blk_found = true;
                                continue;
                            }
                            Err(e) => {
                                kprintln!("[rust] storage: init_storage FAILED @ {:#x}: {:?}", addr, e);
                            }
                        }
                    }
                }
            } else if is_pcie_ecam {
                if let Some(reg) = node.reg().and_then(|mut r| r.next()) {
                    ecam_base = Some(reg.starting_address as usize);
                    kprintln!("[rust] found PCIe ECAM @ {:#x}", ecam_base.unwrap());
                }
            }
        }
    }

    if !net_found { kprintln!("[rust] net: no virtio-net found in DTB"); }
    if !blk_found { kprintln!("[rust] storage: no virtio-blk found in DTB"); }

    // 3. Initialize filesystem
    if blk_found {
        if cotan_fs::init().is_err() { 
            kprintln!("[rust] fs init failed"); 
        }
    } else {
        kprintln!("[rust] fs: skipping init (no storage)");
    }

    // 4. Initialize Power & Thermal management
    kprintln!("[rust] initializing power and thermal management...");
    thermal::check_thermal_status();

    // 5. Enumerate PCIe devices
    if let Some(ecam) = ecam_base {
        let _devices = pcie::enumerate_pcie(ecam);
    } else {
        kprintln!("[rust] pcie: no ECAM found in DTB, skipping enumeration");
    }

    // 6. Test advanced drivers: DMA, IOMMU, RTC
    kprintln!("[rust] testing advanced drivers (DMA, IOMMU, RTC)...");
    test_advanced_drivers();

    let _ = cotan_usb::init_usb(0x00000000);
    kprintln!("[rust] core init done");
    0
}

/// Demonstrates the usage of DMA, IOMMU, and the hardware-independent RTC driver.
fn test_advanced_drivers() {
    // 1. Test DMA Allocator
    let dma = IdentityDmaAllocator::new();
    if let Ok((phys, virt)) = dma.alloc(4096, 4096) {
        kprintln!("[test] DMA allocated: phys={:#x}, virt={:p}", phys, virt);
        dma.free(phys, virt, 4096);
    }

    // 2. Test IOMMU
    let mut iommu = StubIommu::new();
    let _ = iommu.map(0x81000000, 0x81000000, 4096, 0x3); // Read/Write
    let _ = iommu.enable();

    // 3. Test RTC Driver (using the I2C HAL stub)
    let i2c_stub = StubI2cDevice::new(0);
    let mut rtc = Pcf8563Rtc::new(i2c_stub);
    
    // Set a dummy time
    let dummy_time = RtcTime {
        year: 2026,
        month: 11,
        day: 21,
        hour: 14,
        minute: 30,
        second: 0,
    };
    
    if rtc.set_time(&dummy_time).is_ok() {
        kprintln!("[test] RTC time set successfully");
    }
    
    if let Ok(time) = rtc.get_time() {
        kprintln!("[test] RTC read time: {:04}-{:02}-{:02} {:02}:{:02}:{:02}", 
                  time.year, time.month, time.day, time.hour, time.minute, time.second);
    }

    kprintln!("[rust] advanced drivers test complete.");
}
