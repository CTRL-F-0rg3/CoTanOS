// SPDX-FileCopyrightText: 2026 CTRL-F-0rg3
// SPDX-License-Identifier: GPL-3.0-only
//! CoTanOS 256xtr — serial console through CoTanSBI (DBCN extension).

use core::arch::asm;

/// SBI DBCN putchar.
pub fn console_putc(c: u8) {
    unsafe {
        asm!(
            "ecall",
            in("a0") c as usize,
            in("a6") 0usize,
            in("a7") 0x4442_434Eusize, // "DBCN"
            options(nostack)
        );
    }
}

/// SBI DBCN getchar: next character or -1 when the receiver is empty.
pub fn console_getc() -> i64 {
    let c: i64;
    unsafe {
        asm!(
            "ecall",
            in("a6") 2usize,
            in("a7") 0x4442_434Eusize, // "DBCN"
            lateout("a0") c,
            options(nostack)
        );
    }
    c
}

pub fn console_puts(s: &str) {
    for b in s.bytes() {
        console_putc(b);
    }
}

pub fn put_hex(v: u64) {
    console_puts("0x");
    for i in (0..16).rev() {
        let d = ((v >> (i * 4)) & 0xF) as usize;
        console_putc(b"0123456789abcdef"[d]);
    }
}

pub fn put_dec(mut v: u64) {
    let mut buf = [0u8; 20];
    let mut i = 0;
    if v == 0 {
        buf[i] = b'0';
        i += 1;
    }
    while v > 0 {
        buf[i] = b'0' + (v % 10) as u8;
        i += 1;
        v /= 10;
    }
    while i > 0 {
        i -= 1;
        console_putc(buf[i]);
    }
}
