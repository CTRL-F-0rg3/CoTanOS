// SPDX-FileCopyrightText: 2026 CTRL-F-0rg3
// SPDX-License-Identifier: GPL-3.0-only
//! CoTanOS 256xtr — embedded initrd (cpio "newc" archive).
//!
//! The archive is packed by `new/usp/Makefile` and embedded into the kernel
//! image at compile time (single file, no storage driver needed yet).

/// The packed userspace root filesystem.
static INITRD: &[u8] = include_bytes!("../initrd.cpio");

fn rd_hex(b: &[u8]) -> usize {
    let mut v = 0usize;
    for &c in b.iter().take(8) {
        v = v * 16 + (c as char).to_digit(16).unwrap_or(0) as usize;
    }
    v
}

/// Looks up a file in the archive; returns its content.
pub fn lookup(path: &[u8]) -> Option<&'static [u8]> {
    let mut pos = 0usize;
    let data = INITRD;
    while pos + 110 <= data.len() {
        if &data[pos..pos + 6] != b"070701" {
            return None;
        }
        let namesize = rd_hex(&data[pos + 94..pos + 102]);
        let filesize = rd_hex(&data[pos + 54..pos + 62]);
        let name_start = pos + 110;
        if name_start + namesize > data.len() {
            return None;
        }
        let name = &data[name_start..name_start + namesize - 1]; // drop NUL
        // GNU cpio strips the leading "./": accept both "bin/init"
        // and "./bin/init", and compare against the absolute path.
        let clean = name.strip_prefix(b"./").unwrap_or(name);
        let is_match = if clean.starts_with(b"/") {
            clean == path
        } else {
            path.starts_with(b"/") && &path[1..] == clean
        };
        if is_match {
            let fstart = (name_start + namesize + 3) & !3;
            return Some(&data[fstart..fstart + filesize]);
        }
        if clean == b"TRAILER!!!" {
            return None;
        }
        let fstart = (name_start + namesize + 3) & !3;
        pos = (fstart + filesize + 3) & !3;
    }
    None
}
