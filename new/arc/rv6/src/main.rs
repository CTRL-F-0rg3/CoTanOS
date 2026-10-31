// SPDX-FileCopyrightText: 2026 CTRL-F-0rg3
// SPDX-License-Identifier: GPL-3.0-only
//! CoTanOS 256xtr — RISC-V kernel entry (`arc-rv6`).
//!
//! Started by the `ini/` bootloader in S-mode with:
//!   a0 = hart id, a1 = FDT address, a2 = &BootInfo.
//!
//! Brings up the console (SBI), the `cotanos-mem` frame allocator, its own
//! Sv39 page tables, the Linux-compatible syscall layer (U-mode userspace)
//! and starts `/bin/init` from the embedded initrd.

#![no_std]
#![no_main]

extern crate alloc;

use core::arch::{asm, global_asm};
use core::panic::PanicInfo;

use cotanos_mem::prelude::*;

mod console;
mod elf;
mod initrd;
mod kheap;
mod page;
mod proc;
mod syscalls;
mod vfs;

// ============================================================================
// Boot protocol (matches ini/bootinfo.h, version 2)
// ============================================================================

/// Boot information passed by the `ini/` bootloader in `a2`.
#[repr(C)]
#[derive(Clone, Copy)]
struct BootInfo {
    magic: u64,
    version: u64,
    mem_base: u64,
    mem_size: u64,
    fdt_addr: u64,
    kernel_entry: u64,
    fb_addr: u64,
    fb_width: u32,
    fb_height: u32,
    fb_bpp: u32,
    fb_pitch: u32,
}

/// "CoTaBoot"
const BOOTINFO_MAGIC: u64 = 0x436F_5461_426F_6F74;

// ============================================================================
// Early statics (.bss)
// ============================================================================

/// Boot stack for the kernel (lives in .bss, zeroed by `_start`).
#[no_mangle]
static mut STACK: [u8; 64 * 1024] = [0; 64 * 1024];

/// Kernel stack used during traps (sscratch).
#[no_mangle]
static mut KTRAP_STACK: [u8; 64 * 1024] = [0; 64 * 1024];

/// Timer tick counter, incremented by the assembly trap vector.
#[no_mangle]
pub static mut TIMER_TICKS: u64 = 0;

/// Framebuffer base (from BootInfo) — used once to clear the spinner.
static mut FB: *mut u32 = core::ptr::null_mut();

/// Log every syscall on the console (boot debugging; off by default).
pub const DEBUG_SYSCALLS: bool = true;

/// Bitmap backing storage for the physical frame allocator.
/// Frames are indexed from physical address 0, so the bitmap must cover
/// `[0, mem_base + mem_size)`: 12 Ki words = 768 Ki bits = 3 GiB of RAM.
static mut FRAME_BITMAP: [u64; 12 * 1024] = [0; 12 * 1024];

extern "C" {
    fn __kernel_end();
}

pub fn halt() -> ! {
    loop {
        unsafe {
            asm!("wfi", options(nomem, nostack));
        }
    }
}

/// Clears the boot spinner area (the userspace takes over from here).
fn spinner_clear() {
    unsafe {
        let fb = FB;
        if fb.is_null() {
            return;
        }
        for y in 386..=454 {
            for x in 286..=354 {
                fb.add((y * 640 + x) as usize).write(0);
            }
        }
    }
}

// ============================================================================
// Entry + trap vector (assembly)
// ============================================================================

global_asm!(
    r#"
    .section .text.entry
    .global _start
_start:
    # Set up the kernel stack (top of the STACK static).
    la   sp, STACK
    li   t0, 65536
    add  sp, sp, t0

    # Zero .bss (everything up to __kernel_end).
    la   t0, __bss_start
    la   t1, __kernel_end
1:  bgeu t0, t1, 2f
    sd   zero, 0(t0)
    addi t0, t0, 8
    j    1b
2:
    # a0 = hart id, a1 = FDT, a2 = &BootInfo (from the bootloader).
    call kmain
.Lhang:
    wfi
    j .Lhang
"#
);

global_asm!(
    r#"
    .section .text
    .align 4
    .global trap_entry
trap_entry:
    # Swap sp with sscratch: we land on the kernel trap stack, the user sp
    # is parked in sscratch (from kernel mode this is a no-op).
    csrrw sp, sscratch, sp

    # Save the whole context into TRAP_FRAME (x0 is unused, x5/t0 slot
    # holds the frame base — t0 is caller-saved, harmless).
    la   t0, TRAP_FRAME
    sd   x1,    8(t0)
    csrr x1, sscratch
    sd   x1,   16(t0)
    sd   x3,   24(t0)
    sd   x4,   32(t0)
    sd   x5,   40(t0)
    sd   x6,   48(t0)
    sd   x7,   56(t0)
    sd   x8,   64(t0)
    sd   x9,   72(t0)
    sd   x10,  80(t0)
    sd   x11,  88(t0)
    sd   x12,  96(t0)
    sd   x13, 104(t0)
    sd   x14, 112(t0)
    sd   x15, 120(t0)
    sd   x16, 128(t0)
    sd   x17, 136(t0)
    sd   x18, 144(t0)
    sd   x19, 152(t0)
    sd   x20, 160(t0)
    sd   x21, 168(t0)
    sd   x22, 176(t0)
    sd   x23, 184(t0)
    sd   x24, 192(t0)
    sd   x25, 200(t0)
    sd   x26, 208(t0)
    sd   x27, 216(t0)
    sd   x28, 224(t0)
    sd   x29, 232(t0)
    sd   x30, 240(t0)
    sd   x31, 248(t0)
    csrr t1, sepc
    sd   t1, 256(t0)
    csrr t1, sstatus
    sd   t1, 264(t0)

    # Dispatch on scause.
    csrr t1, scause
    bgez t1, .Lexception

    # Interrupt: only the timer (STIP, code 5) is expected here.
    andi t1, t1, 0x7ff
    li   t2, 5
    bne  t1, t2, .Lbad

    # Tick counter (uptime, PRNG seed).
    la   t1, TIMER_TICKS
    ld   t2, 0(t1)
    addi t2, t2, 1
    sd   t2, 0(t1)

    # Reprogram the timer: mtimecmp = mtime + ~26 ms (0x40000 @ 10 MHz).
    li   t1, 0x2000000
    li   t2, 0xBFF8
    add  t1, t1, t2
    ld   t1, 0(t1)
    li   t2, 0x40000
    add  a0, t1, t2
    li   a7, 0x54494D45          # SBI TIME
    li   a6, 0
    ecall
    j    .Lrestore

.Lexception:
    andi t1, t1, 0x7ff
    li   t2, 8                    # ecall from U-mode = syscall
    beq  t1, t2, .Lsyscall
    j    .Lfault

.Lsyscall:
    la   t1, TRAP_FRAME
    ld   t2, 256(t1)
    addi t2, t2, 4                # sepc += 4
    sd   t2, 256(t1)
    call syscall_entry
    j    .Lrestore

.Lfault:
    call fault_entry
    j    .Lrestore

.Lbad:
    csrr a0, scause
    csrr a1, sepc
    call trap_panic
    j    .Lrestore

.Lrestore:
    la   t6, TRAP_FRAME
    ld   t0, 256(t6)
    csrw sepc, t0
    ld   t5, 264(t6)             # sstatus value (parked in t5)
    ld   ra,    8(t6)
    ld   gp,   24(t6)
    ld   tp,   32(t6)
    ld   a0,   80(t6)
    ld   a1,   88(t6)
    ld   a2,   96(t6)
    ld   a3,  104(t6)
    ld   a4,  112(t6)
    ld   a5,  120(t6)
    ld   a6,  128(t6)
    ld   a7,  136(t6)
    ld   s2,  144(t6)
    ld   s3,  152(t6)
    ld   s4,  160(t6)
    ld   s5,  168(t6)
    ld   s6,  176(t6)
    ld   s7,  184(t6)
    ld   s8,  192(t6)
    ld   s9,  200(t6)
    ld   s10, 208(t6)
    ld   s11, 216(t6)
    ld   t3,  224(t6)
    ld   t4,  232(t6)
    ld   t0,   40(t6)
    ld   t1,   48(t6)
    ld   t2,   56(t6)
    ld   s0,   64(t6)
    ld   s1,   72(t6)
    csrw sscratch, sp             # park the kernel trap sp for next time
    ld   sp,   16(t6)
    ld   t6,  248(t6)
    # Restore sstatus with SIE clear; sret then sets SIE <- SPIE
    # atomically. Never re-enable interrupts before sret: a timer tick
    # in that window would re-trap and clobber TRAP_FRAME mid-restore.
    andi t5, t5, -3               # clear SIE (bit 1)
    csrw sstatus, t5
    sret
"#
);

extern "C" {
    fn trap_entry();
}

/// Called from the trap vector on unexpected S-mode traps; never returns.
#[no_mangle]
extern "C" fn trap_panic(cause: usize, epc: usize) -> ! {
    console::console_puts("\n[256xtr] unexpected S-mode trap! scause=");
    console::put_hex(cause as u64);
    console::console_puts(" sepc=");
    console::put_hex(epc as u64);
    console::console_puts("\n");
    halt();
}

/// Fatal kernel fault (used by `fault_entry` when SPP = 1).
pub fn trap_panic_fatal() -> ! {
    let scause: usize;
    let sepc: usize;
    unsafe {
        asm!("csrr {0}, scause", out(reg) scause);
        asm!("csrr {0}, sepc", out(reg) sepc);
    }
    trap_panic(scause, sepc)
}

// ============================================================================
// Kernel main
// ============================================================================

#[no_mangle]
extern "C" fn kmain(hartid: u64, _fdt: u64, bi_ptr: *const BootInfo) -> ! {
    // Install the kernel trap vector (S-mode) + the trap stack (sscratch).
    unsafe {
        asm!("csrw stvec, {0}", in(reg) trap_entry as *const () as usize);
        asm!("la t0, KTRAP_STACK");
        asm!("li t1, 65536");
        asm!("add t0, t0, t1");
        asm!("csrw sscratch, t0");
        // SUM = 1: the kernel may access U-mode pages (user copy helpers).
        asm!("csrs sstatus, {0}", in(reg) 1usize << 18, options(nostack));
    }

    console::console_puts("\n[256xtr] CoTanOS kernel (arc-rv6) - hart ");
    console::put_dec(hartid);
    console::console_puts("\n");

    let bi = unsafe { &*bi_ptr };
    if bi.magic != BOOTINFO_MAGIC {
        console::console_puts("[256xtr] bad bootinfo magic - halting\n");
        halt();
    }

    console::console_puts("[256xtr] bootinfo v");
    console::put_dec(bi.version);
    console::console_puts(" RAM: base=");
    console::put_hex(bi.mem_base);
    console::console_puts(" size=");
    console::put_hex(bi.mem_size);
    console::console_puts("\n");

    // ------------------------------------------------------------------
    // Memory management (cotanos-mem): physical frame allocator over RAM.
    // Frames are indexed from physical address 0, so the bitmap covers
    // [0, mem_base + mem_size) and everything below mem_base is reserved.
    // ------------------------------------------------------------------
    let ram_end = bi.mem_base as usize + bi.mem_size as usize;
    let total_frames = ram_end / Size4Kb::SIZE;

    let alloc: &'static PhysicalFrameAllocator<Size4Kb> = {
        #[allow(static_mut_refs)]
        static mut ALLOC: Option<PhysicalFrameAllocator<Size4Kb>> = None;
        // SAFETY: FRAME_BITMAP is exclusively owned by the kernel and is
        // never touched again after this point.
        let bitmap = unsafe { &mut FRAME_BITMAP };
        // SAFETY: the bitmap is valid, exclusively owned and large enough.
        unsafe {
            ALLOC = Some(PhysicalFrameAllocator::new(
                "rv6-ram",
                total_frames,
                bitmap,
            ));
            ALLOC.as_ref().unwrap()
        }
    };

    // Reserve everything below RAM (MMIO) and everything below the end of
    // the kernel image inside RAM (SBI, bootloader, kernel, font atlas).
    alloc.register_reserved_region(
        PhysAddr::new(0),
        bi.mem_base as usize / Size4Kb::SIZE,
    );
    let kernel_end = __kernel_end as *const () as usize;
    console::console_puts("[256xtr] kernel_end=");
    console::put_hex(kernel_end as u64);
    console::console_puts("\n");
    alloc.register_reserved_region(
        PhysAddr::new(bi.mem_base as usize),
        (kernel_end - bi.mem_base as usize).div_ceil(Size4Kb::SIZE),
    );

    // Reserve the font atlas and the ramfb framebuffer as well.
    alloc.register_reserved_region(
        PhysAddr::new(0x8100_0000),
        (33981usize).div_ceil(Size4Kb::SIZE),
    );
    if bi.fb_addr != 0 {
        let fb_bytes =
            (bi.fb_width as usize) * (bi.fb_height as usize) * (bi.fb_bpp as usize / 8);
        alloc.register_reserved_region(
            PhysAddr::new(bi.fb_addr as usize),
            fb_bytes.div_ceil(Size4Kb::SIZE),
        );
    }

    console::console_puts("[256xtr] frame allocator ready (");
    console::console_puts(alloc.name());
    console::console_puts(")\n");

    let stats = alloc.stats();
    console::console_puts("[256xtr] frames: total=");
    console::put_dec(stats.total_frames as u64);
    console::console_puts(" free=");
    console::put_dec(stats.free_frames as u64);
    console::console_puts("\n");

    // ------------------------------------------------------------------
    // Process subsystem: kernel identity page tables + satp switch,
    // kernel heap (alloc), fd table, Linux-compatible syscall ABI.
    // ------------------------------------------------------------------
    unsafe { proc::ALLOCATOR = Some(alloc) };
    proc::init(bi.mem_base as usize, bi.mem_size as usize);
    vfs::init();

    // Enable timer interrupts in S-mode (STIE + SIE).
    unsafe {
        asm!("csrs sie, {0}", in(reg) 1usize << 5, options(nostack));
        asm!("csrs sstatus, {0}", in(reg) 1usize << 1, options(nostack));
    }

    // The boot spinner ran until here — userspace takes over the screen.
    if bi.fb_addr != 0 {
        // SAFETY: the bootloader mapped the framebuffer 1:1 for the kernel.
        unsafe { FB = bi.fb_addr as *mut u32 };
        spinner_clear();
    }

    console::console_puts("[256xtr] userspace: Linux syscall ABI (riscv64)\n");
    console::console_puts("[256xtr] loading /bin/init from the initrd...\n");

    match proc::exec(
        b"/bin/init",
        &[b"/bin/init".to_vec()],
        &[b"PATH=/bin".to_vec(), b"TERM=vt100".to_vec()],
    ) {
        Ok(()) => {
            let tf = proc::trap_frame();
            proc::enter_user(tf.sepc, tf.regs[2]);
        }
        Err(()) => {
            console::console_puts("[256xtr] /bin/init not found in initrd - halting\n");
            halt();
        }
    }
}

// ============================================================================
// Panic handling
// ============================================================================

#[panic_handler]
fn panic(info: &PanicInfo) -> ! {
    console::console_puts("\n[256xtr] PANIC: ");
    if let Some(msg) = info.message().as_str() {
        console::console_puts(msg);
    } else {
        console::console_puts("<no message>");
    }
    console::console_puts("\n");
    halt();
}

