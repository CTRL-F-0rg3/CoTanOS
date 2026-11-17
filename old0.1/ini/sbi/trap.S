/* SPDX-FileCopyrightText: 2026 CTRL-F-0rg3
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * CoTanSBI — M-mode trap handler.
 * Serves ECALL from S-mode (SBI) and unexpected traps.
 * M-mode interrupts are disabled (mie = 0); the timer is delegated to S.
 */

    .option norvc
    .section .text
    .align 4
    .global sbi_trap

sbi_trap:
    /* Remember the ORIGINAL trap cause before anything (the handler's own
     * stack stores can fault and re-trap, destroying mcause). */
    csrr t0, mcause
    la   t1, sbi_last_trap
    sd   t0, 0(t1)
    csrr t0, mepc
    sd   t0, 8(t1)
    csrr t0, mtval
    sd   t0, 16(t1)
    csrr t0, mcause
    bltz t0, .Lfatal           /* interrupt in M-mode? must not happen */

    /* Exception — only ecall from S-mode (code 9) is handled. */
    andi t0, t0, 0x7ff
    li t1, 9                   /* ecall from S-mode -> SBI */
    bne t0, t1, .Lfatal

    /* mepc += 4 (everything is 4 bytes, .option norvc). */
    csrr t1, mepc
    addi t1, t1, 4
    csrw mepc, t1

    /*
     * C call: uint64_t sbi_handle_ecall(eid, fid, arg)
     *   eid  = a7 (SBI extension id)
     *   fid  = a6 (SBI function id)
     *   arg  = a0 (first argument)
     */
    mv t2, a0
    mv a0, a7
    mv a1, a6
    mv a2, t2
    addi sp, sp, -32
    sd ra, 0(sp)
    call sbi_handle_ecall
    ld ra, 0(sp)
    addi sp, sp, 32
    mret

.Lfatal:
    tail sbi_fatal_trap         /* void sbi_fatal_trap(void) — does not return */

    .size sbi_trap, . - sbi_trap

