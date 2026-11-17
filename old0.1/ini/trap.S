/* SPDX-FileCopyrightText: 2026 CTRL-F-0rg3
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * CoTanBoot — early S-mode trap handler (before the kernel installs its own).
 * Handles the timer interrupt (STIP) and reports the remaining traps.
 */

    .option norvc
    .section .text
    .align 4
    .global boot_trap

boot_trap:
    csrr t0, scause
    bgez t0, .Lfatal           /* exception — unexpected at this stage */

    /* Interrupt: only the timer (code 5, STIP) is handled. */
    andi t0, t0, 0x7ff
    li t1, 5
    bne t0, t1, .Lfatal

    /*
     * Save the interrupted context registers (a trap handler must be
     * transparent: everything it modifies is pushed onto the stack).
     */
    addi sp, sp, -48
    sd t1, 8(sp)
    sd a0, 16(sp)
    sd a6, 24(sp)
    sd a7, 32(sp)

    /* Reprogram mtimecmp = mtime + 1 MiB (~0.1 s @ 10 MHz). */
    li t0, 0x2000000           /* CLINT */
    li t1, 0xBFF8              /* mtime */
    add t0, t0, t1
    ld t0, 0(t0)
    li t1, 0x100000
    add t0, t0, t1
    mv a0, t0
    li a7, 0x54494D45          /* SBI EXT TIME */
    li a6, 0
    ecall                      /* served by CoTanSBI in M-mode */

    /* Restore the registers and return to the interrupted code. */
    ld t1, 8(sp)
    ld a0, 16(sp)
    ld a6, 24(sp)
    ld a7, 32(sp)
    addi sp, sp, 48
    sret

.Lfatal:
    csrr a0, scause
    csrr a1, sepc
    tail boot_fatal_trap       /* does not return */

    .size boot_trap, . - boot_trap
