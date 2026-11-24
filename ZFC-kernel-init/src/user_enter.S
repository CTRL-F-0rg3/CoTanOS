/*
 * user_enter.S - a0 = trapframe *, a1 = user root PT physical address.
 * Layout of trapframe (u64 array):
 *   tf[0] (offset 0): sepc
 *   tf[1] (offset 8): sp
 */
.section .text
.globl user_enter
user_enter:
    ld      t0, 0(a0)       # t0 = sepc
    csrw    sepc, t0
    ld      sp, 8(a0)       # sp = user stack pointer

    li      t0, (1 << 8)
    csrc    sstatus, t0     # SPP = 0 -> U-mode
    li      t0, (1 << 5)
    csrs    sstatus, t0     # SPIE = 1

    li      t0, 8
    slli    t0, t0, 60      # t0 = 8 << 60 (MODE=Sv39 for 64-bit)
    srli    t1, a1, 12
    or      t0, t0, t1
    csrw    satp, t0
    sfence.vma zero, zero

    # Zero all registers to prevent kernel state leakage
    li      ra, 0
    li      gp, 0
    li      tp, 0
    li      s0, 0
    li      s1, 0
    li      s2, 0
    li      s3, 0
    li      s4, 0
    li      s5, 0
    li      s6, 0
    li      s7, 0
    li      s8, 0
    li      s9, 0
    li      s10, 0
    li      s11, 0
    li      a0, 0
    li      a1, 0
    li      a2, 0
    li      a3, 0
    li      a4, 0
    li      a5, 0
    li      a6, 0
    li      a7, 0
    li      t0, 0
    li      t1, 0
    li      t2, 0
    li      t3, 0
    li      t4, 0
    li      t5, 0
    li      t6, 0
    sret
