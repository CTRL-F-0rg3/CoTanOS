.option nopic
.section .text
.globl _start
_start:
    li a0, 6
    li a1, 7
    mul a2, a0, a1
    li a3, 42
    bne a2, a3, f1

    li a0, -3
    li a1, 4
    mul a2, a0, a1
    li a3, -12
    bne a2, a3, f2

    # MULH (signed): -1 * -1 = 1, high = 0
    li a0, -1
    li a1, -1
    mulh a2, a0, a1
    li a3, 0
    bne a2, a3, f3

    # MULH (signed): (-2^63) * 2 = -2^64, high 64 = -1
    li a0, 1
    slli a0, a0, 63
    li a1, 2
    mulh a2, a0, a1
    li a3, -1
    bne a2, a3, f4

    # MULHU (unsigned): 2^63 * 2 = 2^64, high 64 = 1
    li a0, 1
    slli a0, a0, 63
    li a1, 2
    mulhu a2, a0, a1
    li a3, 1
    bne a2, a3, f5

    # MULHSU: -1 (signed) * 0xFFFFFFFFFFFFFFFF (unsigned) = -1 * (2^64-1)
    # = -(2^64 - 1) = -2^64 + 1. High 64 = -1
    li a0, -1
    li a1, -1
    mulhsu a2, a0, a1
    li a3, -1
    bne a2, a3, f6

    # DIV: 20/3=6
    li a0, 20
    li a1, 3
    div a2, a0, a1
    li a3, 6
    bne a2, a3, f7

    # DIV: -20/3=-6
    li a0, -20
    li a1, 3
    div a2, a0, a1
    li a3, -6
    bne a2, a3, f8

    # DIV/0 = -1
    li a0, 42
    li a1, 0
    div a2, a0, a1
    li a3, -1
    bne a2, a3, f9

    # REM: 20%3=2
    li a0, 20
    li a1, 3
    rem a2, a0, a1
    li a3, 2
    bne a2, a3, f10

    # REM: -20%3=-2
    li a0, -20
    li a1, 3
    rem a2, a0, a1
    li a3, -2
    bne a2, a3, f11

    # MULW: 0x8000 * 0x10000 = 0x80000000 → sign-ext → 0xFFFFFFFF80000000
    li a0, 0x8000
    li a1, 0x10000
    mulw a2, a0, a1
    li a3, 0x80000000
    slli a3, a3, 32
    srai a3, a3, 32
    bne a2, a3, f12

    # DIVW: -7/2=-3
    li a0, -7
    li a1, 2
    divw a2, a0, a1
    li a3, -3
    bne a2, a3, f13

    # DIVUW: 0xFFFFFFFF/2 = 0x7FFFFFFF
    li a0, -1
    li a1, 2
    divuw a2, a0, a1
    li a3, 0x7FFFFFFF
    bne a2, a3, f14

    # REMUW: 0xFFFFFFFF % 2 = 1
    li a0, -1
    li a1, 2
    remuw a2, a0, a1
    li a3, 1
    bne a2, a3, f15

    la t0, tohost
    li t1, 1
    sd t1, 0(t0)
1:  j 1b

f1:  li t1, 101; j rpt
f2:  li t1, 102; j rpt
f3:  li t1, 103; j rpt
f4:  li t1, 104; j rpt
f5:  li t1, 105; j rpt
f6:  li t1, 106; j rpt
f7:  li t1, 107; j rpt
f8:  li t1, 108; j rpt
f9:  li t1, 109; j rpt
f10: li t1, 110; j rpt
f11: li t1, 111; j rpt
f12: li t1, 112; j rpt
f13: li t1, 113; j rpt
f14: li t1, 114; j rpt
f15: li t1, 115; j rpt

rpt:
    la t0, tohost
    sd t1, 0(t0)
2:  j 2b

.section .tohost, "aw", @progbits
.align 3
.hidden tohost
tohost: .dword 0
