//! A userspace PS/2 mouse driver.
//!
//! The kernel exposes *raw bytes* through [`TGS_MOUSE_RAW`]; this module is the
//! driver: it assembles the 4-byte PS/2 packets and decodes them into
//! `(dx, dy, buttons)`. Nothing here depends on the kernel's `editor/mouse.c`
//! — the decode that used to live in ring 0 now lives here, in ring 3.

use crate::sysnum::TGS_MOUSE_RAW;

/// Read one raw PS/2 mouse byte from the kernel. `None` when none is queued.
pub fn mouse_raw() -> Option<u8> {
    #[cfg(target_arch = "x86_64")]
    unsafe {
        let v: u64;
        // SAFETY: `int 0x80` is installed at DPL 3 and this call carries no
        // pointers. The kernel returns the byte (or `u64::MAX`) in `rax`.
        core::arch::asm!(
            "int 0x80",
            inlateout("rax") TGS_MOUSE_RAW => v,
            options(nostack),
        );
        if v == u64::MAX {
            None
        } else {
            Some(v as u8)
        }
    }

    #[cfg(not(target_arch = "x86_64"))]
    {
        let _ = TGS_MOUSE_RAW;
        None
    }
}

/// The PS/2 packet size (IntelliMouse, as enabled by the kernel).
const PACKET_LEN: usize = 4;

/// A PS/2 mouse packet decoder.
///
/// Bytes arrive one at a time from [`mouse_raw`]; the first byte of a packet
/// has bit 3 set, which is the sync marker the state machine keys on. An
/// out-of-sync byte is dropped until a sync byte shows up.
#[derive(Debug, Default)]
pub struct Mouse {
    pkt: [u8; PACKET_LEN],
    idx: u8,
}

impl Mouse {
    pub const fn new() -> Self {
        Mouse {
            pkt: [0; PACKET_LEN],
            idx: 0,
        }
    }

    /// Drain queued bytes and return the next complete packet, if one finished.
    ///
    /// Returns `None` when the controller has no more bytes; a partial packet
    /// is kept in the state machine for the next call. At most 16 bytes are
    /// drained per call so a stuck controller cannot spin the caller.
    pub fn poll(&mut self) -> Option<(i32, i32, u32)> {
        for _ in 0..16 {
            let b = mouse_raw()?;
            // The first byte of a packet carries the sync bit (bit 3).
            if self.idx == 0 && b & 0x08 == 0 {
                continue;
            }
            self.pkt[self.idx as usize] = b;
            self.idx += 1;
            if self.idx as usize == PACKET_LEN {
                self.idx = 0;
                return Some(Self::decode(self.pkt));
            }
        }
        None
    }

    /// Decode a full 4-byte PS/2 packet into `(dx, dy, buttons)`.
    ///
    /// Pure — separated out so the host tests can exercise the protocol
    /// without a kernel. Matches the PS/2 sign-extension convention: bit 4 of
    /// byte 0 is the X sign bit, bit 5 the Y sign bit.
    pub fn decode(pkt: [u8; PACKET_LEN]) -> (i32, i32, u32) {
        let buttons = (pkt[0] & 0x07) as u32;
        let dx = pkt[1] as i32 - (((pkt[0] as i32) << 4) & 0x100);
        let dy = pkt[2] as i32 - (((pkt[0] as i32) << 3) & 0x100);
        (dx, dy, buttons)
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn decodes_rightward_motion() {
        // byte0: sync(0x08) | left button(0x01) = 0x09
        let (dx, dy, btn) = Mouse::decode([0x09, 5, 0, 0]);
        assert_eq!(dx, 5);
        assert_eq!(dy, 0);
        assert_eq!(btn, 1);
    }

    #[test]
    fn decodes_negative_motion() {
        // byte0: sync(0x08) | X sign(0x10) | Y sign(0x20) = 0x38
        let (dx, dy, _) = Mouse::decode([0x38, 0xFF, 0xFE, 0]);
        assert_eq!(dx, -1); // 0xFF - 0x100
        assert_eq!(dy, -2); // 0xFE - 0x100
    }

    #[test]
    fn decodes_negative_y_motion() {
        // Y sign bit set, byte2 = 0xFE => -2
        let (_, dy, _) = Mouse::decode([0x28, 0, 0xFE, 0]);
        assert_eq!(dy, -2);
    }

    #[test]
    fn buttons_mask() {
        // sync + left + right + middle = 0x08 | 0x07 = 0x0F
        let (_, _, btn) = Mouse::decode([0x0F, 0, 0, 0]);
        assert_eq!(btn, 0x07);
    }
}
