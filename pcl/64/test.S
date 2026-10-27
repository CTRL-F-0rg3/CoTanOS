.option nopic
.section .text
.globl _start
_start:
    li a0, 5
    li a1, 7
    add a2, a0, a1
    li a3, 12
    bne a2, a3, fail
    la t0, tohost
    li t1, 1
    sd t1, 0(t0)
1:  j 1b

fail:
    la t0, tohost
    li t1, 2
    sd t1, 0(t0)
2:  j 2b

.section .tohost, "aw", @progbits
.align 3
.hidden tohost
tohost:
    .dword 0
