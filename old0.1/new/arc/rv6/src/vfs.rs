// SPDX-FileCopyrightText: 2026 CTRL-F-0rg3
// SPDX-License-Identifier: GPL-3.0-only
//! CoTanOS 256xtr — minimal synthetic VFS (fd table + /proc/self/cmdline).
//!
//! Enough for the Linux userspace ABI: Rust `std::env::args()` reads
//! `/proc/self/cmdline` through openat/read/close — exactly what this
//! module serves.

use crate::console::{console_getc, console_putc};
use crate::proc::proc;

const MAX_FDS: usize = 16;

#[derive(Clone, Copy, PartialEq)]
pub enum FdKind {
    Stdin,
    Stdout,
    Stderr,
    Cmdline,
}

#[derive(Clone, Copy)]
pub struct Fd {
    pub kind: FdKind,
    pub pos: usize,
}

static mut FDS: [Option<Fd>; MAX_FDS] = [None; MAX_FDS];

pub fn init() {
    unsafe {
        FDS[0] = Some(Fd { kind: FdKind::Stdin, pos: 0 });
        FDS[1] = Some(Fd { kind: FdKind::Stdout, pos: 0 });
        FDS[2] = Some(Fd { kind: FdKind::Stderr, pos: 0 });
    }
}

fn get(fd: usize) -> Option<Fd> {
    if fd >= MAX_FDS {
        return None;
    }
    unsafe { FDS[fd] }
}

pub fn write(fd: usize, buf: &[u8]) -> isize {
    match get(fd) {
        Some(f) if f.kind == FdKind::Stdout || f.kind == FdKind::Stderr => {
            for &b in buf {
                console_putc(b);
            }
            buf.len() as isize
        }
        _ => -9, // EBADF
    }
}

/// Reads from a fd. Stdin blocks until a character arrives.
pub fn read(fd: usize, buf: &mut [u8]) -> isize {
    match get(fd) {
        Some(mut f) => match f.kind {
            FdKind::Stdin => {
                for b in buf.iter_mut() {
                    let mut c = console_getc();
                    while c < 0 {
                        c = console_getc();
                    }
                    *b = c as u8;
                }
                buf.len() as isize
            }
            FdKind::Cmdline => {
                let cmd = &proc().cmdline;
                let n = buf.len().min(cmd.len() - f.pos.min(cmd.len()));
                buf[..n].copy_from_slice(&cmd[f.pos..f.pos + n]);
                f.pos += n;
                unsafe { FDS[fd] = Some(f) };
                n as isize
            }
            _ => -9,
        },
        None => -9,
    }
}

pub fn open(path: &[u8]) -> isize {
    if path == b"/proc/self/cmdline" {
        for i in 3..MAX_FDS {
            let free = unsafe { FDS[i].is_none() };
            if free {
                unsafe {
                    FDS[i] = Some(Fd { kind: FdKind::Cmdline, pos: 0 });
                }
                return i as isize;
            }
        }
        return -24; // EMFILE
    }
    -2 // ENOENT
}

pub fn close(fd: usize) -> isize {
    if fd >= 3 && fd < MAX_FDS {
        unsafe { FDS[fd] = None };
        return 0;
    }
    -9
}

pub fn lseek(fd: usize, offset: i64, whence: i32) -> isize {
    match get(fd) {
        Some(mut f) if f.kind == FdKind::Cmdline => {
            let len = proc().cmdline.len() as i64;
            let base = match whence {
                0 => 0,
                1 => f.pos as i64,
                2 => len,
                _ => return -22, // EINVAL
            };
            let np = (base + offset).clamp(0, len) as usize;
            f.pos = np;
            unsafe { FDS[fd] = Some(f) };
            np as isize
        }
        Some(_) => -29, // ESPIPE
        None => -9,
    }
}
