//! Szkielet sterownika xHCI (USB 3.0)
//! Specyfikacja: eXtensible Host Controller Interface (xHCI) 1.2

use cotan_kapi::{KResult, Error};
use core::ptr::{read_volatile, write_volatile};

#[repr(C)]
pub struct XhciController {
    base: usize,
    slots: u8,
    ready: bool,
}

impl XhciController {
    pub const fn new() -> Self {
        Self { base: 0, slots: 0, ready: false }
    }

    pub fn init(&mut self, mmio_base: usize) -> KResult<()> {
        if mmio_base == 0 {
            log::warn!("usb: no xHCI base provided, skipping");
            return Err(Error::NotImplemented);
        }
        self.base = mmio_base;
        log::info!("usb: probing xHCI @ {:#x}", mmio_base);

        // 1. Odczyt Capability Registers (offset 0x00)
        // CAPLENGTH (8 bit) + HCIVERSION (16 bit)
        let cap_length = unsafe { read_volatile(self.base as *const u8) } as usize;
        let hci_version = unsafe { read_volatile((self.base + 2) as *const u16) };
        log::info!("usb: xHCI version: {:x}.{:x}, CapLength: {}", 
                   (hci_version >> 8) & 0xF, hci_version & 0xF, cap_length);

        // 2. Odczyt HCSParams1 (offset 0x04) - liczba slotów (urządzeń)
        let hcs1 = unsafe { read_volatile((self.base + 4) as *const u32) };
        self.slots = (hcs1 & 0xFF) as u8;
        log::info!("usb: max device slots: {}", self.slots);

        // 3. Twardy reset kontrolera (Operational Registers, offset = cap_length)
        let op_base = self.base + cap_length;
        let mut usbcmd = unsafe { read_volatile(op_base as *const u32) };
        
        // Ustaw bit HCRST (Host Controller Reset) = bit 1
        usbcmd |= 0x0000_0002;
        unsafe { write_volatile(op_base as *mut u32, usbcmd) };
        log::info!("usb: asserting HCRST...");

        // Czekaj na zakończenie resetu (bit HCRST musi spaść do 0)
        let mut timeout = 100000;
        while timeout > 0 {
            usbcmd = unsafe { read_volatile(op_base as *const u32) };
            if (usbcmd & 0x0000_0002) == 0 {
                break;
            }
            timeout -= 1;
            // W prawdziwym kernelu: core::hint::spin_loop()
        }

        if timeout == 0 {
            log::error!("usb: xHCI reset timeout!");
            return Err(Error::Io);
        }

        log::info!("usb: xHCI reset successful");
        self.ready = true;
        Ok(())
    }

    pub fn is_ready(&self) -> bool { self.ready }
}

pub static XHCI: spin::Mutex<XhciController> = spin::Mutex::new(XhciController::new());
