# trap.s – minimal S-mode trap vector, prints diagnostics then halts
.section .text.trap, "ax", @progbits

.balign 4
.globl trap_vector
trap_vector:
    csrr    a0, scause
    csrr    a1, sepc
    csrr    a2, stval
    call    c_trap_handler
.Ltrap_hang:
    wfi
    j       .Ltrap_hang

.globl trap_init
trap_init:
    la      t0, trap_vector
    csrw    stvec, t0
    ret
