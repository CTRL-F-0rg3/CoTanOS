.section .text
.global _start
.type _start, @function

_start:
    mv s0, a0
    mv s1, a1
    la sp, _stack_top
    call kernel_main_riscv
3:
    wfi
    j 3b

.section .bss
.align 4
_stack_top:
