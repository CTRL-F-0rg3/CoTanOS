.option nopic
.section .text
.globl _start
_start:
    # RAW: a0 zależy od poprzedniego wyniku
    li a0, 10
    addi a0, a0, 5      # a0 = 15
    addi a0, a0, 5      # a0 = 20 (RAW)
    addi a0, a0, 5      # a0 = 25
    li a1, 25
    bne a0, a1, f1

    # load-use: lw → następna instrukcja używa wyniku
    la s0, scratch
    li t0, 0xDEADBEEF
    sd t0, 0(s0)
    ld t1, 0(s0)        # load
    add t2, t1, t1      # use → load-use stall
    li t3, 0xDEADBEEF
    add t4, t3, t3
    bne t2, t4, f2

    # forward z EX/MEM do EX (1 instr gap)
    li a0, 100
    addi a1, a0, 1      # forward z EX/MEM
    addi a2, a1, 1      # forward z MEM/WB
    li a3, 102
    bne a2, a3, f3

    la t0, tohost
    li t1, 1
    sd t1, 0(t0)
1:  j 1b

f1:  li t1, 301; j rpt
f2:  li t1, 302; j rpt
f3:  li t1, 303; j rpt

rpt:
    la t0, tohost
    sd t1, 0(t0)
2:  j 2b

.section .data
.align 3
scratch: .space 16

.section .tohost, "aw", @progbits
.align 3
.hidden tohost
tohost: .dword 0
