.option nopic
.section .text
.globl _start
_start:
    la s0, scratch       # bazowy adres scratch

    # --- SD / LD ---
    li t0, 0x123456789ABCDEF0
    sd t0, 0(s0)
    ld t1, 0(s0)
    bne t0, t1, fail_31

    # --- SW / LW ---
    li t0, 0xDEADBEEF
    sw t0, 8(s0)
    lw t1, 8(s0)
    li t2, 0xFFFFFFFFDEADBEEF   # sign-extended LW
    bne t1, t2, fail_32

    # --- LWU (zero-extend) ---
    lwu t1, 8(s0)
    li t2, 0x00000000DEADBEEF
    bne t1, t2, fail_33

    # --- SH / LH ---
    li t0, 0x8000
    sh t0, 16(s0)
    lh t1, 16(s0)
    li t2, 0xFFFFFFFFFFFF8000   # sign-extended
    bne t1, t2, fail_34

    # --- LHU ---
    lhu t1, 16(s0)
    li t2, 0x8000
    bne t1, t2, fail_35

    # --- SB / LB ---
    li t0, 0x80
    sb t0, 24(s0)
    lb t1, 24(s0)
    li t2, 0xFFFFFFFFFFFFFF80
    bne t1, t2, fail_36

    # --- LBU ---
    lbu t1, 24(s0)
    li t2, 0x80
    bne t1, t2, fail_37

    # pass
    la t0, tohost
    li t1, 1
    sd t1, 0(t0)
1:  j 1b

fail_31: li t1, 31; j report
fail_32: li t1, 32; j report
fail_33: li t1, 33; j report
fail_34: li t1, 34; j report
fail_35: li t1, 35; j report
fail_36: li t1, 36; j report
fail_37: li t1, 37; j report

report:
    la t0, tohost
    sd t1, 0(t0)
2:  j 2b

.section .data
.align 3
scratch: .space 32

.section .tohost, "aw", @progbits
.align 3
.hidden tohost
tohost: .dword 0
