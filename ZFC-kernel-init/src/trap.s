# trap.S - S-mode trap vector with per-hart trap stack.
# Convention: sscratch holds this hart's trap stack top at all times.

.section .text.trap, "ax", @progbits
.balign 4
.globl trap_vector
trap_vector:
    # Swap sp with sscratch: sp = kernel trap stack, sscratch = trapped sp.
    csrrw   sp, sscratch, sp
    addi    sp, sp, -128
    sd      ra, 0(sp)
    csrr    t0, sscratch
    sd      t0, 8(sp)               # save trapped sp
    sd      a0, 16(sp)
    sd      a1, 24(sp)
    sd      a2, 32(sp)

    # Restore sscratch to the trap stack top for the next trap.
    addi    t0, sp, 128
    csrw    sscratch, t0

    csrr    a0, scause
    csrr    a1, sepc
    csrr    a2, stval
    call    c_trap_handler
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
    csrw    sscratch, t1
    ret

.section .bss
.align 4
.globl g_trap_stacks
g_trap_stacks:
    .space 0x1000 * 8               # up to 8 harts
