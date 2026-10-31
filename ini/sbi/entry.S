/* SPDX-FileCopyrightText: 2026 CTRL-F-0rg3
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * CoTanSBI — M-mode firmware entry point.
 * QEMU (reset vector @ 0x1000) jumps here with:
 *   a0 = mhartid
 *   a1 = Flattened Device Tree (FDT) address
 */

    .option norvc
    .section .text.entry
    .global _start

_start:
    csrr a0, mhartid          /* a0 = hart number (0 = primary hart) */

    /* Stack for all harts; non-zero harts are parked. */
    la sp, __stack_top
    bnez a0, .Lpark

    /* --- zero .bss (hart 0) --- */
    la t0, __bss_start
    la t1, __bss_end
.Lbss:
    bgeu t0, t1, .Lbss_done
    sd zero, 0(t0)
    addi t0, t0, 8
    j .Lbss
.Lbss_done:

    /* Remember the FDT address (a1) — it is forwarded to the bootloader. */
    mv s0, a1

    /* mtvec -> our M-mode trap handler (direct mode). */
    la t0, sbi_trap
    csrw mtvec, t0

    /*
     * medeleg — delegate the usual exceptions to S-mode:
     *   0  instruction-addr-misaligned
     *   3  breakpoint
     *   5  load access fault
     *   7  store access fault
     *   8  ecall from U-mode
     *   12 instr page fault
     *   13 load page fault
     *   15 store page fault
     * NOTE: bit 9 (ecall from S-mode) is NOT delegated —
     * that is how SBI works: ecall from S-mode lands in M-mode.
     */
    li t0, 0xB1A9
    csrw medeleg, t0

    /*
     * mideleg — delegate these interrupts to S-mode:
     *   1 (SSIP), 5 (STIP, timer), 9 (SEIP, external)
     */
    li t0, 0x222
    csrw mideleg, t0

    /* mcounteren: let S-mode read cycle/time. */
    li t0, 0x3
    csrw mcounteren, t0

    /* PMP: open all memory (RWX, TOR, range 0..2^64). */
    li t0, -1
    csrw pmpaddr0, t0
    li t0, 0x0F
    csrw pmpcfg0, t0

    /* mstatus: MPP = S-mode (01 -> bit 11 = 0x800), everything else cleared. */
    li t0, 0x800
    csrw mstatus, t0

    /* M-mode takes no interrupts (the timer is delegated to S). */
    csrw mie, zero

    /* C: sbi_main(hartid, fdt_addr) — a0/a1 are ready. */
    call sbi_main

    /* sbi_main does not return (it does mret into the bootloader). */
    j .Lhalt

.Lpark:
    /* Harts other than 0 wait in a loop. */
    csrw mie, zero
.Lpark_loop:
    wfi
    j .Lpark_loop

.Lhalt:
    wfi
    j .Lhalt

    .size _start, . - _start
