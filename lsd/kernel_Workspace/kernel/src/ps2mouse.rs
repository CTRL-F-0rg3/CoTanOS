//! Minimal PS/2 mouse *port* access — raw bytes only, no decoding.
//!
//! This is the kernel's half of the userspace mouse driver: it enables the
//! PS/2 auxiliary device once and then hands out whatever raw bytes the
//! controller produces. The packet decode (assembly, sign extension, button
//! mask) lives in userspace (`tgs-hal/src/mouse.rs`), so this module is a
//! *separate* driver from the legacy `editor/mouse.c` path served by
//! `SYS_MOUSE`; nothing here calls into that C code.

use core::sync::atomic::{AtomicBool, Ordering};

#[inline]
unsafe fn inb(port: u16) -> u8 {
    let v: u8;
    core::arch::asm!("in al, dx", out("al") v, in("dx") port);
    v
}

#[inline]
unsafe fn outb(port: u16, val: u8) {
    core::arch::asm!("out dx, al", in("dx") port, in("al") val);
}

fn wait_write() {
    while unsafe { inb(0x64) } & 0x02 != 0 {
        core::hint::spin_loop();
    }
}

fn wait_read() {
    while unsafe { inb(0x64) } & 0x01 == 0 {
        core::hint::spin_loop();
    }
}

fn mouse_cmd(cmd: u8) {
    wait_write();
    unsafe { outb(0x64, 0xD4) };
    wait_write();
    unsafe { outb(0x60, cmd) };
    wait_read();
    unsafe { inb(0x60) };
}

static INIT: AtomicBool = AtomicBool::new(false);

/// Enable the PS/2 auxiliary device and data reporting. Idempotent.
pub fn init_once() {
    if INIT.swap(true, Ordering::Relaxed) {
        return;
    }
    wait_write();
    unsafe { outb(0x64, 0xA8) }; // enable the auxiliary device
    mouse_cmd(0xF6);             // set defaults
    mouse_cmd(0xF4);             // enable data reporting
}

/// Read one raw PS/2 mouse byte, if one is sitting in the output buffer.
pub fn raw_byte() -> Option<u8> {
    let st = unsafe { inb(0x64) };
    if st & 0x01 == 0 {
        return None; // output buffer empty
    }
    // Re-read status: bit 5 of the byte we are about to consume tells whether
    // it came from the mouse (aux) or the keyboard.
    let st = unsafe { inb(0x64) };
    if st & 0x20 == 0 {
        return None; // keyboard data, not mouse
    }
    Some(unsafe { inb(0x60) })
}
