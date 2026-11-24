.section .text.init, "ax"
.globl _start
_start:
    .option push
    .option norelax
    la      gp, __global_pointer$
    .option pop
    call    main
    mv      a0, a0
    li      a7, 93          # SYS_exit
    ecall
1:  j       1b
