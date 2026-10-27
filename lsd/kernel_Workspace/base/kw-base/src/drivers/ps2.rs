use crate::arch::x86_64::{inb, outb};

const PS2_DATA: u16 = 0x60;
const PS2_STATUS: u16 = 0x64;
const PS2_CMD: u16 = 0x64;

pub struct Ps2Controller;

impl Ps2Controller {
    pub fn init() {
        // Disable devices
        unsafe {
            outb(PS2_CMD, 0xAD);
            outb(PS2_CMD, 0xA7);

            // Flush output buffer
            while (inb(PS2_STATUS) & 0x01) != 0 { inb(PS2_DATA); }

            // Enable keyboard interface (clears the inhibit/lock bit)
            outb(PS2_CMD, 0xAE);

            // Write command byte: enable keyboard IRQ (bit 0) and keyboard clock (bit 4)
            outb(PS2_CMD, 0x60);
            outb(PS2_DATA, 0x11);

            // Enable devices (aux/mouse)
            outb(PS2_CMD, 0xA8);

            // Enable scanning at the keyboard device
            outb(PS2_DATA, 0xF4);
        }
    }

    pub fn read_scancode() -> Option<u8> {
        unsafe {
            if (inb(PS2_STATUS) & 0x01) != 0 {
                Some(inb(PS2_DATA))
            } else {
                None
            }
        }
    }
}