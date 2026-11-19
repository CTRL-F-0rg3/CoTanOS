/* SPDX-FileCopyrightText: 2026 CTRL-F-0rg3
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * CoTanBoot — bootloader entry (S-mode).
 * Entered via mret from CoTanSBI:
 *   a0 = hartid
 *   a1 = adres FDT
 */

    .option norvc
    .section .text.entry
    .global _start

_start:
    mv s0, a0                 /* hartid */
    mv s1, a1                 /* adres FDT */

    la sp, __stack_top

    /* Early S-mode trap handler (timer / traps until the kernel takes over). */
    la t0, boot_trap
    csrw stvec, t0

    /* --- zero .bss (together with the page-table buffers) --- */
    la t0, __bss_start
    la t1, __bss_end
.Lbss:
    bgeu t0, t1, .Lbss_done
    sd zero, 0(t0)
    addi t0, t0, 8
    j .Lbss
.Lbss_done:

    /* C: void init_main(uint64_t hartid, uint64_t fdt) */
    mv a0, s0
    mv a1, s1
    call init_main

    /* init_main jumps to the kernel and does not return. */
.Lhalt:
    wfi
    j .Lhalt

    .size _start, . - _start
