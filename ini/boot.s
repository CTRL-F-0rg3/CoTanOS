.equ STACK_SIZE, 4096

.section .text.init
.global _start

_start:
    # 1. Konfiguracja stosu dla rdzenia 0 (hart 0)
    la sp, stack_top

    # 2. Wyczyszczenie sekcji BSS
    la t0, __bss_start
    la t1, __bss_end
b_clear:
    bge t0, t1, b_done
    sd zero, 0(t0)
    addi t0, t0, 8
    j b_clear

b_done:
    # 3. Skok do głównej procedury inicjalizacyjnej (C)
    call init_main

    # 4. Bezpieczna pętla zatrzymująca
_halt:
    wfi
    j _halt

.section .bss
.align 16
stack_bottom:
    .space STACK_SIZE * 4
stack_top:
