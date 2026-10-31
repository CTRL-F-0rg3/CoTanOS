// SPDX-FileCopyrightText: 2026 CTRL-F-0rg3
// SPDX-License-Identifier: GPL-3.0-only
//! CoTanOS 256xtr — syscall layer.
//!
//! **Linux-compatible syscall ABI on RISC-V**: `ecall` from U-mode,
//! `a7` = syscall number, args in `a0..a5`, return value in `a0`
//! (negative = `-errno`). Because the ABI is the Linux one, ordinary
//! `riscv64-linux` static binaries run unchanged: glibc/musl C, C++ with
//! libstdc++ and Rust with the real, precompiled `std`
//! (`riscv64gc-unknown-linux-gnu`).

use alloc::vec::Vec;

use crate::proc::{self, trap_frame};

// --- Linux riscv64 syscall numbers (asm-generic table) ---------------------
const SYS_GETCWD: usize = 17;
const SYS_FCNTL: usize = 25;
const SYS_IOCTL: usize = 29;
const SYS_OPENAT: usize = 56;
const SYS_CLOSE: usize = 57;
const SYS_READ: usize = 63;
const SYS_WRITE: usize = 64;
const SYS_EXIT: usize = 93;
const SYS_EXIT_GROUP: usize = 94;
const SYS_SET_TID_ADDRESS: usize = 96;
const SYS_FUTEX: usize = 98;
const SYS_SET_ROBUST_LIST: usize = 99;
const SYS_NANOSLEEP: usize = 101;
const SYS_CLOCK_GETTIME: usize = 113;
const SYS_SCHED_YIELD: usize = 124;
const SYS_KILL: usize = 129;
const SYS_TKILL: usize = 130;
const SYS_TGKILL: usize = 131;
const SYS_RT_SIGACTION: usize = 134;
const SYS_RT_SIGPROCMASK: usize = 135;
const SYS_UNAME: usize = 160;
const SYS_GETTIMEOFDAY: usize = 169;
const SYS_GETPID: usize = 172;
const SYS_GETPPID: usize = 173;
const SYS_GETUID: usize = 174;
const SYS_GETEUID: usize = 175;
const SYS_GETGID: usize = 176;
const SYS_GETEGID: usize = 177;
const SYS_GETTID: usize = 178;
const SYS_SYSINFO: usize = 179;
const SYS_BRK: usize = 214;
const SYS_MUNMAP: usize = 215;
const SYS_EXECVE: usize = 221;
const SYS_MMAP: usize = 222;
const SYS_MPROTECT: usize = 226;
const SYS_GETRANDOM: usize = 278;
const SYS_RSEQ: usize = 293;

// mmap / prot flags
const PROT_READ: usize = 1;
const PROT_WRITE: usize = 2;
const PROT_EXEC: usize = 4;
const MAP_PRIVATE: usize = 0x02;
const MAP_FIXED: usize = 0x10;
const MAP_ANONYMOUS: usize = 0x20;

/// Called from the assembly trap vector for every `ecall` from U-mode.
#[no_mangle]
extern "C" fn syscall_entry() {
    let tf = trap_frame();
    let (n, a0, a1, a2, a3, a4, a5) = (
        tf.regs[17], tf.regs[10], tf.regs[11], tf.regs[12], tf.regs[13], tf.regs[14], tf.regs[15],
    );
    let ret = dispatch(n, a0, a1, a2, a3, a4, a5);
    trap_frame().regs[10] = ret as usize;
}

fn dispatch(n: usize, a0: usize, a1: usize, a2: usize, a3: usize, a4: usize, a5: usize) -> isize {
    if crate::DEBUG_SYSCALLS {
        crate::console::console_puts("[sys] ");
        crate::console::put_dec(n as u64);
        crate::console::console_puts("(");
        crate::console::put_hex(a0 as u64);
        crate::console::console_puts(",");
        crate::console::put_hex(a1 as u64);
        crate::console::console_puts(",");
        crate::console::put_hex(a2 as u64);
        crate::console::console_puts(")\n");
    }
    match n {
        SYS_READ => sys_read(a0, a1, a2),
        SYS_WRITE => sys_write(a0, a1, a2),
        SYS_OPENAT => sys_openat(a0, a1, a2),
        SYS_CLOSE => crate::vfs::close(a0),
        SYS_EXIT | SYS_EXIT_GROUP => {
            proc::kill_process(a0 as i64);
            0
        }
        SYS_SET_TID_ADDRESS => sys_set_tid_address(a0),
        SYS_FUTEX => sys_futex(a0, a1, a2),
        SYS_SET_ROBUST_LIST => 0,
        SYS_NANOSLEEP => sys_nanosleep(a0, a1),
        SYS_CLOCK_GETTIME => sys_clock_gettime(a0, a1),
        SYS_SCHED_YIELD => 0,
        SYS_KILL | SYS_TKILL | SYS_TGKILL => {
            proc::kill_process(0);
            0
        }
        SYS_RT_SIGACTION | SYS_RT_SIGPROCMASK => 0,
        SYS_UNAME => sys_uname(a0),
        SYS_GETTIMEOFDAY => sys_gettimeofday(a0, a1),
        SYS_GETPID | SYS_GETTID => 1,
        SYS_GETPPID => 0,
        SYS_GETUID | SYS_GETEUID | SYS_GETGID | SYS_GETEGID => 0,
        SYS_SYSINFO => sys_sysinfo(a0),
        SYS_BRK => proc::brk(a0) as isize,
        SYS_MUNMAP => {
            proc::unmap_range(a0, a1);
            0
        }
        SYS_EXECVE => sys_execve(a0, a1, a2),
        SYS_MMAP => sys_mmap(a0, a1, a2, a3, a4, a5),
        SYS_MPROTECT => proc::mprotect(a0, a1, a2 as u8),
        SYS_GETRANDOM => sys_getrandom(a0, a1),
        SYS_GETCWD => sys_getcwd(a0, a1),
        SYS_FCNTL => 0,
        SYS_IOCTL => -25, // ENOTTY
        258 => 0, // membarrier: nothing to register (glibc retries on ENOSYS)
        SYS_RSEQ => -38,   // ENOSYS (glibc falls back gracefully)
        _ => {
            // Everything else: report on the console once, return ENOSYS.
            crate::console::console_puts("[256xtr] unknown syscall ");
            crate::console::put_dec(n as u64);
            crate::console::console_puts("\n");
            -38
        }
    }
}

// ---------------------------------------------------------------------------
// Implementations
// ---------------------------------------------------------------------------

fn sys_write(fd: usize, buf: usize, len: usize) -> isize {
    if !proc::user_range_ok(buf, len) {
        return -14; // EFAULT
    }
    // SAFETY: range validated against mapped user pages.
    let slice = unsafe { core::slice::from_raw_parts(buf as *const u8, len) };
    crate::vfs::write(fd, slice)
}

fn sys_read(fd: usize, buf: usize, len: usize) -> isize {
    if !proc::user_range_ok(buf, len) {
        return -14;
    }
    // SAFETY: range validated against mapped user pages.
    let slice = unsafe { core::slice::from_raw_parts_mut(buf as *mut u8, len) };
    crate::vfs::read(fd, slice)
}

fn sys_openat(_dirfd: usize, path: usize, _flags: usize) -> isize {
    match proc::u_str(path, 256) {
        Some(p) => crate::vfs::open(&p),
        None => -14,
    }
}

fn sys_set_tid_address(addr: usize) -> isize {
    let z = [0u8; 4];
    if !proc::u_write(addr, &z) {
        return -14;
    }
    1
}

fn sys_futex(_addr: usize, op: usize, _val: usize) -> isize {
    // FUTEX_WAIT (0) / WAIT_BITSET (9): a spurious wakeup is always legal,
    // so return immediately. FUTEX_WAKE (1): nothing to wake — one thread.
    match op & 0x7F {
        0 | 9 => 0,
        1 => 0,
        _ => -22,
    }
}

fn sys_nanosleep(req: usize, _rem: usize) -> isize {
    let mut b = [0u8; 16];
    if !proc::u_read(req, &mut b) {
        return -14;
    }
    let sec = i64::from_le_bytes(b[0..8].try_into().unwrap());
    let nsec = i64::from_le_bytes(b[8..16].try_into().unwrap());
    let ns = sec * 1_000_000_000 + nsec.max(0);
    let ticks = (ns / 100).max(1) as u64; // mtime is 10 MHz
    let target = proc::mtime() + ticks;
    while proc::mtime() < target {
        unsafe { core::arch::asm!("wfi", options(nomem, nostack)) };
    }
    0
}

fn sys_clock_gettime(clk: usize, tp: usize) -> isize {
    // CLOCK_MONOTONIC (1), CLOCK_REALTIME (0) and friends: all from mtime.
    if clk > 8 {
        return -22;
    }
    let ns = proc::mtime() * 100;
    let mut b = [0u8; 16];
    b[0..8].copy_from_slice(&(ns / 1_000_000_000).to_le_bytes());
    b[8..16].copy_from_slice(&(ns % 1_000_000_000).to_le_bytes());
    if !proc::u_write(tp, &b) {
        return -14;
    }
    0
}

fn sys_gettimeofday(tv: usize, tz: usize) -> isize {
    if tv != 0 {
        let ns = proc::mtime() * 100;
        let mut b = [0u8; 16];
        b[0..8].copy_from_slice(&(ns / 1_000_000_000).to_le_bytes());
        b[8..16].copy_from_slice(&(ns % 1_000_000_000 / 1000).to_le_bytes());
        if !proc::u_write(tv, &b) {
            return -14;
        }
    }
    if tz != 0 {
        let z = [0u8; 8];
        if !proc::u_write(tz, &z) {
            return -14;
        }
    }
    0
}

fn sys_uname(buf: usize) -> isize {
    // struct utsname: six 65-byte fields.
    let mut out = [0u8; 390];
    let fill = |out: &mut [u8; 390], off: usize, s: &str| {
        let b = s.as_bytes();
        let n = b.len().min(64);
        out[off..off + n].copy_from_slice(&b[..n]);
    };
    fill(&mut out, 0, "CoTanOS");
    fill(&mut out, 65, "cotanos");
    fill(&mut out, 130, "0.1.0");
    fill(&mut out, 195, "256xtr");
    fill(&mut out, 260, "riscv64");
    if !proc::u_write(buf, &out) {
        return -14;
    }
    0
}

fn sys_sysinfo(buf: usize) -> isize {
    let stats = proc::allocator().stats();
    let mut out = [0u8; 112];
    // SAFETY: the tick counter is only bumped by the timer trap.
    let ticks = unsafe { crate::TIMER_TICKS };
    let uptime = (ticks * 26) / 1000;
    out[0..8].copy_from_slice(&uptime.to_le_bytes());
    out[8..16].copy_from_slice(&(1u64).to_le_bytes()); // loadavg
    out[16..24].copy_from_slice(&((stats.total_frames * 4096) as u64).to_le_bytes());
    out[24..32].copy_from_slice(&((stats.free_frames * 4096) as u64).to_le_bytes());
    out[32..40].copy_from_slice(&0u64.to_le_bytes()); // sharedram
    out[40..48].copy_from_slice(&0u64.to_le_bytes()); // bufferram
    out[48..56].copy_from_slice(&0u64.to_le_bytes()); // totalswap
    out[56..64].copy_from_slice(&0u64.to_le_bytes()); // freeswap
    out[64..66].copy_from_slice(&1u16.to_le_bytes()); // procs
    out[72..80].copy_from_slice(&0u64.to_le_bytes()); // totalhigh
    out[80..88].copy_from_slice(&0u64.to_le_bytes()); // freehigh
    out[88..92].copy_from_slice(&1u32.to_le_bytes()); // mem_unit
    if !proc::u_write(buf, &out) {
        return -14;
    }
    0
}

fn sys_getrandom(buf: usize, len: usize) -> isize {
    if !proc::user_range_ok(buf, len) {
        return -14;
    }
    // SAFETY: the tick counter is only bumped by the timer trap.
    let ticks = unsafe { crate::TIMER_TICKS };
    let mut s = (proc::mtime() ^ ticks as u64 ^ 0x9E37_79B9_7F4A_7C15) | 1;
    let mut out: Vec<u8> = Vec::with_capacity(len);
    for _ in 0..len {
        s ^= s << 13;
        s ^= s >> 7;
        s ^= s << 17;
        out.push((s >> 32) as u8);
    }
    if !proc::u_write(buf, &out) {
        return -14;
    }
    len as isize
}

fn sys_getcwd(buf: usize, size: usize) -> isize {
    if size < 2 {
        return -34; // ERANGE
    }
    let b = [b'/', 0u8];
    if !proc::u_write(buf, &b) {
        return -14;
    }
    buf as isize
}

fn sys_execve(path: usize, argv: usize, envp: usize) -> isize {
    let path = match proc::u_str(path, 256) {
        Some(p) => p,
        None => return -14,
    };
    let argv = match proc::u_str_array(argv, 32) {
        Some(a) => a,
        None => return -14,
    };
    let envp = match proc::u_str_array(envp, 32) {
        Some(e) => e,
        None => return -14,
    };
    match proc::exec(&path, &argv, &envp) {
        Ok(()) => 0,
        Err(()) => -8, // ENOEXEC
    }
}

fn sys_mmap(addr: usize, len: usize, prot: usize, flags: usize, _fd: usize, _off: usize) -> isize {
    if flags & MAP_ANONYMOUS == 0 {
        // File-backed mappings are not supported yet.
        return -38; // ENOSYS
    }
    if prot & !(PROT_READ | PROT_WRITE | PROT_EXEC) != 0 {
        return -22;
    }
    let fixed = flags & MAP_FIXED != 0;
    proc::mmap_anon(addr, len, prot as u8, fixed)
}
