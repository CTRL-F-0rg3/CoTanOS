// SPDX-FileCopyrightText: 2026 CTRL-F-0rg3
// SPDX-License-Identifier: GPL-3.0-only
//! CoTanOS 256xtr — process model v1 (single process + Linux ABI userspace).
//!
//! One address space at a time: kernel identity (RAM + MMIO) plus one user
//! program loaded from the embedded initrd. `exec` replaces the whole user
//! half of the page table (this is how `/bin/init` chains the C, C++ and
//! Rust demo programs without `fork`).

use alloc::vec::Vec;

use core::arch::asm;

use cotanos_mem::prelude::{PhysFrame, PhysicalFrameAllocator, Size4Kb};

use crate::console::console_puts;
use crate::page::{PageTable, PTE_A, PTE_D, PTE_U, PTE_V, PTE_R, PTE_W, PTE_X};

/// Global frame allocator handle (set once at boot).
pub static mut ALLOCATOR: Option<&'static PhysicalFrameAllocator<Size4Kb>> = None;

// ---------------------------------------------------------------------------
// User address space layout (Sv39, user half = [0, 0x4000_0000_00))
// ---------------------------------------------------------------------------

/// Top of the user stack (16 pages below it are mapped).
pub const STACK_TOP: usize = 0x3FC0_0000_00;
pub const STACK_PAGES: usize = 16;

/// mmap search area: above any ET_DYN image (1 GiB), below the kernel VAs
/// (kernel identity starts at 0x8000_0000 = 2 GiB).
pub const MMAP_MIN: usize = 0x2_0000_0000;
pub const MMAP_MAX: usize = 0x7000_0000;

pub const USER_HALF: usize = 0x4000_0000_00;

// ---------------------------------------------------------------------------
// Trap frame (the assembly trap vector saves/restores the whole context here)
// ---------------------------------------------------------------------------

#[repr(C)]
#[derive(Clone, Copy)]
pub struct TrapFrame {
    pub regs: [usize; 32],
    pub sepc: usize,
    pub sstatus: usize,
}

#[no_mangle]
static mut TRAP_FRAME: TrapFrame = TrapFrame { regs: [0; 32], sepc: 0, sstatus: 0 };

pub fn trap_frame() -> &'static mut TrapFrame {
    // SAFETY: the trap frame is a kernel-owned static, touched only from
    // trap context (interrupts disabled) — no data races.
    unsafe { &mut TRAP_FRAME }
}

// ---------------------------------------------------------------------------
// Process
// ---------------------------------------------------------------------------

#[derive(Clone, Copy)]
pub struct Region {
    pub vs: usize,
    pub ve: usize,
    pub prot: u8, // 1 = R, 2 = W, 4 = X
}

pub struct Process {
    pub pt: PageTable,
    pub regions: Vec<Region>,
    pub brk: usize,
    pub cmdline: Vec<u8>,
    pub restarts: u32,
}

static mut PROC: Option<Process> = None;

pub fn proc() -> &'static mut Process {
    // SAFETY: single-CPU v1; every use is in trap context (SIE off).
    unsafe { PROC.as_mut().unwrap() }
}

pub fn allocator() -> &'static PhysicalFrameAllocator<Size4Kb> {
    // SAFETY: set once at boot before any userspace activity.
    unsafe { ALLOCATOR.unwrap() }
}

/// Boots the process subsystem: kernel identity page tables + satp switch.
pub fn init(mem_base: usize, mem_size: usize) {
    let alloc = allocator();
    crate::kheap::init(alloc);

    let pt = PageTable::new(alloc).expect("root page table frame");
    // Kernel identity: RAM (2 MiB leaves, RWX).
    let ram_end = mem_base + mem_size;
    let mut va = mem_base & !0x1F_FFFF;
    while va < ram_end {
        let _ = pt.map_2m(va, va, PTE_V | PTE_R | PTE_W | PTE_X | PTE_A | PTE_D, alloc);
        va += 0x200000;
    }
    // MMIO — only what is really needed, mapped as small supervisor pages
    // (a full identity map of [0, 4 GiB) would collide with the user
    // address space, e.g. the ELF load address 0x10000):
    //   * test device (SBI shutdown)        @ 0x100000
    //   * CLINT (mtime for the trap path)   @ 0x2000000..0x200C000
    //   * UART + fw_cfg (SBI console)       @ 0x10000000 (2 MiB leaf)
    let flags_rw = PTE_V | PTE_R | PTE_W | PTE_A | PTE_D;
    let _ = pt.map(0x100000, 0x100000, flags_rw, alloc);
    let mut clint = 0x2000000usize;
    while clint < 0x200C000 {
        let _ = pt.map(clint, clint, flags_rw, alloc);
        clint += 0x1000;
    }
    let _ = pt.map_2m(0x10000000, 0x10000000, flags_rw, alloc);
    pt.activate();

    unsafe {
        PROC = Some(Process {
            pt,
            regions: Vec::new(),
            brk: 0,
            cmdline: Vec::new(),
            restarts: 0,
        });
    }
}

pub fn user_flags(prot: u8) -> usize {
    let mut f = PTE_V | PTE_U | PTE_A | PTE_D;
    if prot & 1 != 0 {
        f |= PTE_R;
    }
    if prot & 2 != 0 {
        f |= PTE_W;
    }
    if prot & 4 != 0 {
        f |= PTE_X;
    }
    f
}


// ---------------------------------------------------------------------------
// User memory helpers (kernel runs with sstatus.SUM = 1)
// ---------------------------------------------------------------------------

/// Checks that `[addr, addr+len)` lies in mapped user pages.
pub fn user_range_ok(addr: usize, len: usize) -> bool {
    if addr == 0 || addr >= USER_HALF {
        return false;
    }
    let end = match addr.checked_add(len) {
        Some(e) => e,
        None => return false,
    };
    if end > USER_HALF {
        return false;
    }
    let mut va = addr & !0xFFF;
    while va < end {
        if proc().pt.flags_of(va) & PTE_U == 0 {
            return false;
        }
        va += 0x1000;
    }
    true
}

pub fn u_read(addr: usize, buf: &mut [u8]) -> bool {
    if !user_range_ok(addr, buf.len()) {
        return false;
    }
    // SAFETY: range checked against mapped user pages; SUM = 1.
    unsafe { core::ptr::copy_nonoverlapping(addr as *const u8, buf.as_mut_ptr(), buf.len()) };
    true
}

pub fn u_write(addr: usize, buf: &[u8]) -> bool {
    if !user_range_ok(addr, buf.len()) {
        return false;
    }
    // SAFETY: range checked against mapped user pages; SUM = 1.
    unsafe { core::ptr::copy_nonoverlapping(buf.as_ptr(), addr as *mut u8, buf.len()) };
    true
}

/// Reads a NUL-terminated user string (up to `max` bytes).
pub fn u_str(addr: usize, max: usize) -> Option<Vec<u8>> {
    let mut out = Vec::new();
    for i in 0..max {
        let mut b = [0u8; 1];
        if !u_read(addr + i, &mut b) {
            return None;
        }
        if b[0] == 0 {
            return Some(out);
        }
        out.push(b[0]);
    }
    None
}

/// Reads an array of user pointers (argv/envp) into owned strings.
pub fn u_str_array(mut addr: usize, max_args: usize) -> Option<Vec<Vec<u8>>> {
    let mut out = Vec::new();
    for _ in 0..max_args {
        let mut w = [0u8; 8];
        if !u_read(addr, &mut w) {
            return None;
        }
        let p = usize::from_le_bytes(w);
        if p == 0 {
            return Some(out);
        }
        out.push(u_str(p, 512)?);
        addr += 8;
    }
    None
}


// ---------------------------------------------------------------------------
// Page mapping operations
// ---------------------------------------------------------------------------

/// Maps `pages` zeroed pages at `va` with `prot` (1=R,2=W,4=X).
pub fn map_user_pages(va: usize, pages: usize, prot: u8) -> Result<(), ()> {
    for i in 0..pages {
        let f = allocator().allocate_frame().ok_or(())?;
        let pa = f.start_address().as_usize();
        unsafe { core::ptr::write_bytes(pa as *mut u8, 0, 0x1000) };
        proc().pt.map(va + i * 0x1000, pa, user_flags(prot), allocator())?;
        proc().regions.push(Region { vs: va + i * 0x1000, ve: va + (i + 1) * 0x1000, prot });
    }
    Ok(())
}

/// Unmaps `[addr, addr+len)` (page aligned) and frees the frames.
pub fn unmap_range(addr: usize, len: usize) {
    let pages = (len + 0xFFF) / 0x1000;
    for i in 0..pages {
        let va = addr + i * 0x1000;
        if let Some(pa) = proc().pt.unmap(va, allocator()) {
            // SAFETY: frame was allocated by us and is no longer referenced.
            unsafe { allocator().deallocate_frame(PhysFrame::from_index(pa / 0x1000)) };
        }
    }
    // Drop the stale regions covering [addr, addr+pages*0x1000).
    let end = addr + pages * 0x1000;
    proc().regions.retain(|r| r.ve <= addr || r.vs >= end);
}

/// Tears down the whole user half (before loading a new image).
fn free_all_user() {
    let regions: Vec<Region> = proc().regions.clone();
    for r in &regions {
        unmap_range(r.vs, r.ve - r.vs);
    }
    proc().regions.clear();
    proc().brk = 0;
}


// ---------------------------------------------------------------------------
// mmap / brk / mprotect
// ---------------------------------------------------------------------------

pub fn mmap_anon(addr: usize, len: usize, prot: u8, fixed: bool) -> isize {
    let pages = (len + 0xFFF) / 0x1000;
    if pages == 0 {
        return -22; // EINVAL
    }
    let va = if fixed {
        addr & !0xFFF
    } else {
        // First fit above MMAP_MIN, below MMAP_MAX.
        let mut cand = MMAP_MIN;
        loop {
            if cand + pages * 0x1000 > MMAP_MAX {
                return -12; // ENOMEM
            }
            let mut free = true;
            for i in 0..pages {
                if proc().pt.flags_of(cand + i * 0x1000) != 0 {
                    free = false;
                    break;
                }
            }
            if free {
                break cand;
            }
            cand += 0x1000;
        }
    };
    if va == 0 || va + pages * 0x1000 > MMAP_MAX || va >= 0x8000_0000 {
        return -12;
    }
    if fixed {
        unmap_range(va, pages * 0x1000);
    }
    if map_user_pages(va, pages, prot).is_err() {
        return -12;
    }
    va as isize
}

pub fn brk(addr: usize) -> usize {
    let p = proc();
    if addr == 0 {
        return p.brk;
    }
    let old = p.brk;
    if addr < old {
        unmap_range(addr, old - addr);
    } else if addr > old {
        let start = (old + 0xFFF) & !0xFFF;
        let end = (addr + 0xFFF) & !0xFFF;
        for va in (start..end).step_by(0x1000) {
            if p.pt.flags_of(va) != 0 {
                continue; // already mapped
            }
            let f = match allocator().allocate_frame() {
                Some(f) => f,
                None => return old,
            };
            let pa = f.start_address().as_usize();
            unsafe { core::ptr::write_bytes(pa as *mut u8, 0, 0x1000) };
            if p.pt.map(va, pa, user_flags(3), allocator()).is_err() {
                return old;
            }
            p.regions.push(Region { vs: va, ve: va + 0x1000, prot: 3 });
        }
    }
    p.brk = addr;
    addr
}

pub fn mprotect(addr: usize, len: usize, prot: u8) -> isize {
    let end = (addr + len + 0xFFF) & !0xFFF;
    let mut va = addr & !0xFFF;
    while va < end {
        if proc().pt.flags_of(va) & PTE_U == 0 {
            return -12;
        }
        proc().pt.set_flags(va, user_flags(prot));
        va += 0x1000;
    }
    0
}

// ---------------------------------------------------------------------------
// exec — replaces the whole user half of the address space
// ---------------------------------------------------------------------------

/// Random 16 bytes for AT_RANDOM (xorshift over mtime + tick).
fn rand16() -> [u8; 16] {
    // SAFETY: the tick counter is only bumped by the timer trap.
    let ticks = unsafe { crate::TIMER_TICKS };
    let mut s = (mtime() ^ ticks as u64) | 1;
    let mut out = [0u8; 16];
    for b in out.iter_mut() {
        s ^= s << 13;
        s ^= s >> 7;
        s ^= s << 17;
        *b = (s >> 32) as u8;
    }
    out
}

/// Current CLINT mtime (10 MHz).
pub fn mtime() -> u64 {
    // SAFETY: CLINT is identity mapped (MMIO region).
    unsafe { core::ptr::read_volatile(0x200BFF8usize as *const u64) }
}

/// Writes a string to the stack area and returns its address.
fn put_str(str_pos: &mut usize, s: &[u8]) -> usize {
    let p = *str_pos;
    for &b in s {
        // SAFETY: the stack pages are mapped for the kernel (SUM = 1).
        unsafe { *(*str_pos as *mut u8) = b };
        *str_pos += 1;
    }
    // SAFETY: as above.
    unsafe { *(*str_pos as *mut u8) = 0 };
    *str_pos += 1;
    p
}

/// Builds the initial user stack (argc/argv/envp/auxv) and returns `sp`.
fn build_stack(
    argv: &[Vec<u8>],
    envp: &[Vec<u8>],
    phdr: usize,
    phent: usize,
    phnum: usize,
    base: usize,
) -> usize {
    // Strings go at the bottom of the mapped stack area.
    let stack_bottom = STACK_TOP - STACK_PAGES * 0x1000;
    let mut str_pos = stack_bottom;

    let rnd = rand16();
    for &b in rnd.iter() {
        // SAFETY: the stack pages are mapped for the kernel (SUM = 1).
        unsafe { *(str_pos as *mut u8) = b };
        str_pos += 1;
    }
    let rnd_ptr = str_pos - 16;

    let argv_ptrs: Vec<usize> = argv.iter().map(|a| put_str(&mut str_pos, a)).collect();
    let envp_ptrs: Vec<usize> = envp.iter().map(|e| put_str(&mut str_pos, e)).collect();

    // Word area grows down from the top.
    let mut sp = STACK_TOP;
    let mut push = |v: usize| {
        sp -= 8;
        // SAFETY: the stack pages are mapped for the kernel (SUM = 1).
        unsafe { *(sp as *mut usize) = v };
    };

    // auxv (highest)
    push(0);
    push(0); // AT_NULL
    push(rnd_ptr);
    push(25); // AT_RANDOM
    push(4096);
    push(6); // AT_PAGESZ
    push(phnum);
    push(5); // AT_PHNUM
    push(phent);
    push(4); // AT_PHENT
    push(phdr);
    push(3); // AT_PHDR
    push(base);
    push(7); // AT_BASE
    push(100);
    push(17); // AT_CLKTCK

    // envp
    push(0);
    for e in envp_ptrs.iter().rev() {
        push(*e);
    }
    // argv
    push(0);
    for a in argv_ptrs.iter().rev() {
        push(*a);
    }
    push(argv_ptrs.len());

    sp & !0xF // 16-byte aligned
}

/// Loads `path` from the initrd, replaces the user address space and points
/// the trap frame at the new program. Returns `Err` when loading fails.
pub fn exec(path: &[u8], argv: &[Vec<u8>], envp: &[Vec<u8>]) -> Result<(), ()> {
    let data = crate::initrd::lookup(path).ok_or(())?;
    console_puts("[256xtr] exec: image ");
    crate::console::put_dec(data.len() as u64);
    console_puts(" bytes\n");

    free_all_user();

    let mut regions = Vec::new();
    let loaded = crate::elf::load(data, &proc().pt, &mut regions).ok_or(())?;
    console_puts("[256xtr] exec: entry=");
    crate::console::put_hex(loaded.entry as u64);
    console_puts("\n");
    // Debug: verify the entry page content landed correctly.
    {
        let pa = proc().pt.pa_of(loaded.entry);
        console_puts("[256xtr] entry pa=");
        crate::console::put_hex(pa as u64);
        console_puts(" bytes:");
        for i in 0..8 {
            let b = unsafe { *((pa + i) as *const u8) };
            crate::console::put_hex(b as u64);
            console_puts(" ");
        }
        console_puts("\n");
    }

    // User stack.
    if map_user_pages(STACK_TOP - STACK_PAGES * 0x1000, STACK_PAGES, 3).is_err() {
        return Err(());
    }
    let sp = build_stack(argv, envp, loaded.phdr, loaded.phent, loaded.phnum, loaded.base);

    // /proc/self/cmdline = NUL-separated argv (Linux format).
    let mut cmd = Vec::new();
    for (i, a) in argv.iter().enumerate() {
        if i > 0 {
            cmd.push(0);
        }
        cmd.extend_from_slice(a);
    }
    cmd.push(0);

    let p = proc();
    p.regions = regions;
    p.brk = loaded.brk;
    p.cmdline = cmd;

    // Point the trap frame at the fresh program: user sp, entry, SPP = 0.
    let tf = trap_frame();
    tf.regs[2] = sp;
    tf.regs[10] = 0; // a0 = 0
    tf.sepc = loaded.entry;
    let keep = tf.sstatus & ((1 << 18) | (1 << 19) | 0x6000); // SUM, MXR, FS
    tf.sstatus = keep | (1 << 5); // SPIE (SIE = 0; sret enables it)
    Ok(())
}

/// First entry into user mode: `sret` with SPP = 0.
pub fn enter_user(entry: usize, sp: usize) -> ! {
    unsafe {
        asm!(
            "csrw sscratch, sp",
            "csrw sepc, {e}",
            // SUM (bit 18) must survive: the kernel copies to/from user
            // pages in the syscall path. SPIE = 1, SIE = 0: sret enables
            // interrupts atomically. FS = 3 (0x6000): the userspace is
            // built with the double-float ABI — without FS the first
            // FP instruction is an illegal-instruction trap.
            "li t0, 0x46020",
            "csrw sstatus, t0",
            "mv sp, {s}",
            "sret",
            e = in(reg) entry,
            s = in(reg) sp,
            options(noreturn)
        );
    }
}

// ---------------------------------------------------------------------------
// Exit / faults — restart init (userspace never stays down)
// ---------------------------------------------------------------------------

/// Terminates the current program (exit syscall, signal or fault) and
/// restarts `/bin/init`. Halts when the restart budget is used up.
pub fn kill_process(code: i64) {
    console_puts("\n[256xtr] user program terminated (code ");
    crate::console::put_dec(code as u64);
    console_puts(")");
    let p = proc();
    p.restarts += 1;
    if p.restarts > 3 {
        console_puts("\n[256xtr] restart budget exhausted — halting\n");
        crate::halt();
    }
    console_puts(" — restarting /bin/init\n");
    if exec(b"/bin/init", &[b"/bin/init".to_vec()], &[b"PATH=/bin".to_vec()]).is_err() {
        console_puts("[256xtr] init restart failed — halting\n");
        crate::halt();
    }
    // The trap frame now points at the fresh init; the caller returns to
    // the assembly restore path, which sret's into it.
}

/// Entry point for unexpected user traps (faults): restart init.
#[no_mangle]
extern "C" fn fault_entry() {
    // In-kernel fault (SPP = 1)? That is a kernel bug — halt.
    let spp = trap_frame().sstatus & (1 << 8) != 0;
    if spp {
        crate::trap_panic_fatal();
    }
    let scause: usize;
    let stval: usize;
    unsafe {
        asm!("csrr {0}, scause", out(reg) scause);
        asm!("csrr {0}, stval", out(reg) stval);
    }
    console_puts("\n[256xtr] user fault: scause=");
    crate::console::put_hex(scause as u64);
    console_puts(" stval=");
    crate::console::put_hex(stval as u64);
    console_puts(" sepc=");
    crate::console::put_hex(trap_frame().sepc as u64);
    console_puts(" tp=");
    crate::console::put_hex(trap_frame().regs[4] as u64);
    console_puts("\n");
    // Debug: dump 96 bytes around tp (TCB) to trace TLS issues.
    let tp = trap_frame().regs[4];
    if tp != 0 {
        for off in (0usize..96).step_by(16) {
            let va = tp - 32 + off;
            let mut line: [u8; 16] = [0; 16];
            crate::proc::u_read(va, &mut line);
            console_puts("  tcb+");
            crate::console::put_hex((tp - 32 + off) as u64);
            console_puts(": ");
            for b in line {
                crate::console::put_hex(b as u64);
                console_puts(" ");
            }
            console_puts("\n");
        }
    }
    kill_process(-1);
}

