# userspace/hello.S - pierwszy program userspace
.section .text
.globl _start

_start:
    # write(1, msg, len)
    li      a7, 1           # SYS_WRITE
    li      a0, 1           # fd = stdout
    la      a1, msg         # buf
    li      a2, 13          # count
    ecall
    
    # exit(0)
    li      a7, 60          # SYS_EXIT
    li      a0, 0
    ecall

.section .rodata
msg:
    .string "Hello World!\n"
