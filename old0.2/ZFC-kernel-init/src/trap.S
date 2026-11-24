/*
 * trap.S - S-mode trap vector with return-to-user path.
 * Convention: while in U-mode, sscratch = this hart's trap frame base
 *             (= trap_stack_top - 320). One frame per hart; kernel
 *             faults are fatal, so no nesting is supported.
 * CoTanOS GPLv3
 */
.section .text.trap, "ax", @progbits
.balign 4
.globl trap_vector
trap_vector:
    csrrw   sp, sscratch, sp        # sp = frame base, sscratch = trapped sp
    csrr    t0, sscratch            # t0 = trapped sp
    sd      t0, 16(sp)
    csrw    sscratch, sp            # sscratch = frame base again

    sd      ra,   8(sp)
    sd      gp,  24(sp)
    sd      tp,  32(sp)
    sd      t0,  40(sp)
    sd      t1,  48(sp)
    sd      t2,  56(sp)
    sd      s0,  64(sp)
    sd      s1,  72(sp)
    sd      a0,  80(sp)
    sd      a1,  88(sp)
    sd      a2,  96(sp)
    sd      a3, 104(sp)
    sd      a4, 112(sp)
    sd      a5, 120(sp)
    sd      a6, 128(sp)
    sd      a7, 136(sp)
    sd      s2, 144(sp)
    sd      s3, 152(sp)
    sd      s4, 160(sp)
    sd      s5, 168(sp)
    sd      s6, 176(sp)
    sd      s7, 184(sp)
    sd      s8, 192(sp)
    sd      s9, 200(sp)
    sd      s10, 208(sp)
    sd      s11, 216(sp)
    sd      t3, 224(sp)
    sd      t4, 232(sp)
    sd      t5, 240(sp)
    sd      t6, 248(sp)

    csrr    t0, scause
    sd      t0, 256(sp)
    csrr    t0, sepc
    sd      t0, 264(sp)
    csrr    t0, stval
    sd      t0, 272(sp)
    csrr    t0, sstatus
    sd      t0, 280(sp)

    mv      a0, sp
    call    cotan_trap_dispatch
    bnez    a0, .Ltrap_hang           # nonzero = fatal

    ld      t0, 264(sp)
    csrw    sepc, t0
    li      t0, (1 << 8)              # SPP = 0 -> U-mode
    csrc    sstatus, t0
    li      t0, (1 << 5)              # SPIE = 1
    csrs    sstatus, t0

    ld      ra,   8(sp)
    ld      gp,  24(sp)
    ld      tp,  32(sp)
    ld      t0,  40(sp)
    ld      t1,  48(sp)
    ld      t2,  56(sp)
    ld      s0,  64(sp)
    ld      s1,  72(sp)
    ld      a0,  80(sp)
    ld      a1,  88(sp)
    ld      a2,  96(sp)
    ld      a3, 104(sp)
    ld      a4, 112(sp)
    ld      a5, 120(sp)
    ld      a6, 128(sp)
    ld      a7, 136(sp)
    ld      s2, 144(sp)
    ld      s3, 152(sp)
    ld      s4, 160(sp)
    ld      s5, 168(sp)
    ld      s6, 176(sp)
    ld      s7, 184(sp)
    ld      s8, 192(sp)
    ld      s9, 200(sp)
    ld      s10, 208(sp)
    ld      s11, 216(sp)
    ld      t3, 224(sp)
    ld      t4, 232(sp)
    ld      t5, 240(sp)
    ld      t6, 248(sp)

    csrrw   sp, sscratch, sp          # sp = user sp, sscratch = frame base
    sret

.Ltrap_hang:
    wfi
    j       .Ltrap_hang

# void trap_init(uint64 hartid)
.section .text
.globl trap_init
trap_init:
    la      t0, trap_vector
    csrw    stvec, t0
    la      t1, g_trap_stacks
    li      t2, 0x1000
    addi    a0, a0, 1
    mul     t2, t2, a0
    add     t1, t1, t2
    addi    t1, t1, -320              # frame lives at top of the stack page
    csrw    sscratch, t1
    ret

.section .bss
.align 4
.globl g_trap_stacks
g_trap_stacks:
    .space 0x1000 * 8
