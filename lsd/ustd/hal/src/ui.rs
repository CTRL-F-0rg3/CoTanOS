//! The framebuffer syscall ([`sysnum::TGS_UI_OPEN`]) and the buffer it maps.
//!
//! This is the one syscall whose *results* come back in three registers
//! (`rdi` = `width<<16 | height`, `rsi` = stride, `rdx` = framebuffer base)
//! rather than the single `rax` that [`crate::gate::Syscall::call`] returns, so
//! it gets its own gate instead of being forced through that trait.

use crate::sysnum::{TGS_KEY, TGS_MOUSE, TGS_UI_OPEN, UI_FONT_VA};

/// A kernel-provided framebuffer mapped into the caller's address space.
#[derive(Debug, Clone, Copy)]
pub struct Framebuffer {
    /// Width in pixels.
    pub width: u32,
    /// Height in pixels.
    pub height: u32,
    /// Row stride in pixels (may exceed `width` when the hardware pads rows).
    pub stride: u32,
    /// Base address of the XRGB8888 pixel buffer.
    pub base: *mut u32,
    /// Base address of the kernel's 8x8 console font (96 glyphs of 8 bytes,
    /// index `ch - 32`, with index 95 holding `?`).
    pub font: *const u8,
}

/// Ask the kernel for the framebuffer via `TGS_UI_OPEN`.
///
/// Returns `None` when there is no display (headless boot, or the driver
/// reported nothing), in which case the kernel answers `u64::MAX` in `rax`.
pub fn ui_open() -> Option<Framebuffer> {
    #[cfg(target_arch = "x86_64")]
    unsafe {
        let mut status: u64;
        let mut rdi: u64;
        let mut rsi: u64;
        let mut rdx: u64;

        // SAFETY: `int 0x80` is installed at DPL 3 during boot and this call
        // carries no pointers. The kernel writes the answer into
        // rax/rdi/rsi/rdx, which are read back through the late outputs below.
        core::arch::asm!(
            "int 0x80",
            inlateout("rax") TGS_UI_OPEN => status,
            lateout("rdi") rdi,
            lateout("rsi") rsi,
            lateout("rdx") rdx,
            options(nostack),
        );

        if status == u64::MAX || rdi == 0 {
            return None;
        }

        Some(Framebuffer {
            width: (rdi >> 16) as u32,
            height: (rdi & 0xFFFF) as u32,
            stride: rsi as u32,
            base: rdx as *mut u32,
            font: UI_FONT_VA as *const u8,
        })
    }

    #[cfg(not(target_arch = "x86_64"))]
    {
        let _ = (TGS_UI_OPEN, UI_FONT_VA);
        None
    }
}

/// Poll the PS/2 mouse via `TGS_MOUSE`. Returns `(dx, dy, buttons)` or `None`
/// when the kernel reports no packet. `dy` is the raw PS/2 value (inverted).
pub fn mouse_poll() -> Option<(i32, i32, u32)> {
    #[cfg(target_arch = "x86_64")]
    unsafe {
        let mut buttons: u64;
        let mut rdi: u64;
        let mut rsi: u64;

        // SAFETY: `int 0x80` is installed at DPL 3 and this call carries no
        // pointers. The kernel writes buttons/rdi/rsi which are read back here.
        core::arch::asm!(
            "int 0x80",
            inlateout("rax") TGS_MOUSE => buttons,
            lateout("rdi") rdi,
            lateout("rsi") rsi,
            options(nostack),
        );

        if buttons == u64::MAX {
            None
        } else {
            Some((rdi as i64 as i32, rsi as i64 as i32, buttons as u32))
        }
    }

    #[cfg(not(target_arch = "x86_64"))]
    {
        let _ = TGS_MOUSE;
        None
    }
}

/// Poll one key from the USB HID keyboard via `TGS_KEY`. `None` when nothing.
pub fn key_poll() -> Option<char> {
    #[cfg(target_arch = "x86_64")]
    unsafe {
        let mut c: u64;

        // SAFETY: `int 0x80` is installed at DPL 3 and this call carries no
        // pointers. The kernel returns the char (0 = nothing) in `rax`.
        core::arch::asm!(
            "int 0x80",
            inlateout("rax") TGS_KEY => c,
            options(nostack),
        );

        if c == 0 {
            None
        } else {
            Some((c as u8) as char)
        }
    }

    #[cfg(not(target_arch = "x86_64"))]
    {
        let _ = TGS_KEY;
        None
    }
}
