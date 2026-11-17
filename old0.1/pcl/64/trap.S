.option nopic
.section .text
.globl _start
_start:
    la t0, handler
    csrw mtvec, t0

    # Test 1: CSR read/write round-trip
    li t0, 0x123456789ABCDEF0
    csrw mscratch, t0
    csrr t1, mscratch
    bne t0, t1, f1

    # Test 2: ecall → handler → mret
    li a0, 0
    ecall
    li a1, 1
    bne a0, a1, f2

    # Test 3: mepc points at ecall+4 after handler
    csrr t0, mcause
    li a1, 11        # ecall from M-mode
    bne t0, a1, f3

    la t0, tohost
    li t1, 1
    sd t1, 0(t0)
1:  j 1b

f1:  li t1, 201; j rpt
f2:  li t1, 202; j rpt
f3:  li t1, 203; j rpt

rpt:
    la t0, tohost
    sd t1, 0(t0)
2:  j 2b

handler:
    addi a0, a0, 1
    csrr t0, mepc
    addi t0, t0, 4
    csrw mepc, t0
    mret

.section .tohost, "aw", @progbits
.align 3
.hidden tohost
tohost: .dword 0
