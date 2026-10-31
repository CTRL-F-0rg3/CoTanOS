// SPDX-FileCopyrightText: 2026 CTRL-F-0rg3
// SPDX-License-Identifier: GPL-3.0-only
//! CoTanOS userspace — /bin/hello-rs (Rust with the REAL, precompiled `std`).
//!
//! Built for `riscv64gc-unknown-linux-gnu` (crt-static). The kernel speaks
//! the Linux syscall ABI, so this is a completely ordinary Rust program.

use std::env;
use std::time::Instant;

/// Raw execve syscall (a7 = 221 on riscv64 Linux).
unsafe fn sys_execve(path: *const u8, argv: *const usize, envp: *const usize) -> isize {
    let r: isize;
    unsafe {
        core::arch::asm!(
            "ecall",
            in("a0") path,
            in("a1") argv,
            in("a2") envp,
            in("a7") 221usize,
            lateout("a0") r,
        );
    }
    r
}

fn main() {
    println!("[rust] Hello from Rust with FULL std (riscv64gc-unknown-linux-gnu)!");

    let args: Vec<String> = env::args().collect();
    println!("[rust] argv via /proc/self/cmdline: {:?}", args);

    let t = Instant::now();
    let v: Vec<u64> = (0..100_000).map(|i| i * i).collect();
    println!(
        "[rust] computed {} squares in {:?} (Instant = clock_gettime)",
        v.len(),
        t.elapsed()
    );

    // Hand back to init for the wrap-up.
    let path = b"/bin/init\0";
    let arg0 = b"/bin/init\0";
    let arg1 = b"done\0";
    let argv: [usize; 3] = [arg0.as_ptr() as usize, arg1.as_ptr() as usize, 0];
    let envp: [usize; 1] = [0];
    let r = unsafe { sys_execve(path.as_ptr(), argv.as_ptr(), envp.as_ptr()) };
    println!("[rust] execve failed: {}", r);
}
