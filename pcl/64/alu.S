.option nopic
.section .text
.globl _start
_start:
    # --- ADD ---
    li a0, 100
    li a1, -1
    add a2, a0, a1
    li a3, 99
    bne a2, a3, fail_11

    # --- SUB ---
    li a0, 50
    li a1, 60
    sub a2, a0, a1
    li a3, -10
    bne a2, a3, fail_12

    # --- AND ---
    li a0, 0xff00
    li a1, 0x0ff0
    and a2, a0, a1
    li a3, 0x0f00
    bne a2, a3, fail_13

    # --- OR ---
    li a0, 0xf0f0
    li a1, 0x0f0f
    or a2, a0, a1
    li a3, 0xffff
    bne a2, a3, fail_14

    # --- XOR ---
    li a0, 0xffff
    li a1, 0x0f0f
    xor a2, a0, a1
    li a3, 0xf0f0
    bne a2, a3, fail_15

    # --- SLL (shift 4) ---
    li a0, 1
    li a1, 4
    sll a2, a0, a1
    li a3, 16
    bne a2, a3, fail_16

    # --- SRL ---
    li a0, 0x100
    li a1, 4
    srl a2, a0, a1
    li a3, 0x10
    bne a2, a3, fail_17

    # --- SRA (arithmetic) ---
    li a0, -16
    li a1, 2
    sra a2, a0, a1
    li a3, -4
    bne a2, a3, fail_18

    # --- SLT (signed) ---
    li a0, -1
    li a1, 1
    slt a2, a0, a1
    li a3, 1
    bne a2, a3, fail_19

    # --- SLTU (unsigned) ---
    li a0, -1
    li a1, 1
    sltu a2, a0, a1
    li a3, 0
    bne a2, a3, fail_20

    # --- ADDI ---
    li a0, 100
    addi a2, a0, -50
    li a3, 50
    bne a2, a3, fail_21

    # --- SLLI ---
    li a0, 3
    slli a2, a0, 2
    li a3, 12
    bne a2, a3, fail_22

    # --- ADDW (32-bit, sign-extend) ---
    li a0, 0x7fffffff
    li a1, 1
    addw a2, a0, a1
    li a3, -2147483648
    bne a2, a3, fail_23

    # --- SUBW ---
    li a0, 0
    li a1, 1
    subw a2, a0, a1
    li a3, -1
    bne a2, a3, fail_24

    # --- SLLW ---
    li a0, 1
    li a1, 3
    sllw a2, a0, a1
    li a3, 8
    bne a2, a3, fail_25

    # --- SRLW ---
    li a0, 0x100
    li a1, 4
    srlw a2, a0, a1
    li a3, 0x10
    bne a2, a3, fail_26

    # --- SRAW ---
    li a0, -16
    li a1, 2
    sraw a2, a0, a1
    li a3, -4
    bne a2, a3, fail_27

    # all pass
    la t0, tohost
    li t1, 1
    sd t1, 0(t0)
1:  j 1b

fail_11: li t1, 11; j report
fail_12: li t1, 12; j report
fail_13: li t1, 13; j report
fail_14: li t1, 14; j report
fail_15: li t1, 15; j report
fail_16: li t1, 16; j report
fail_17: li t1, 17; j report
fail_18: li t1, 18; j report
fail_19: li t1, 19; j report
fail_20: li t1, 20; j report
fail_21: li t1, 21; j report
fail_22: li t1, 22; j report
fail_23: li t1, 23; j report
fail_24: li t1, 24; j report
fail_25: li t1, 25; j report
fail_26: li t1, 26; j report
fail_27: li t1, 27; j report

report:
    la t0, tohost
    sd t1, 0(t0)
2:  j 2b

.section .tohost, "aw", @progbits
.align 3
.hidden tohost
tohost: .dword 0
