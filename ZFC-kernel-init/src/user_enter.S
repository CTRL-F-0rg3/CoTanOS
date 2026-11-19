/*
 * user_enter.S - switch to the process page table and drop to U-mode.
 * a0 = struct trapframe *, a1 = user root page table physical address
 * xv6 trapframe offsets: epc=24, ra=40, sp=48
 */
.section .text
.globl user_enter
user_enter:
    ld      t0, 24(a0)
    csrw    sepc, t0
    ld      ra, 40(a0)
    ld      sp, 48(a0)

    li      t0, (1 << 8)            # SPP = 0 -> U-mode after sret
    csrc    sstatus, t0
    li      t0, (1 << 5)            # SPIE = 1
    csrs    sstatus, t0

    li      t0, 8                   # satp.MODE = Sv39
    slli    t0, t0, 60
    srli    t1, a1, 12
    or      t0, t0, t1
    csrw    satp, t0
    sfence.vma zero, zero

    sret
