# apps/user_test.S - Pierwszy program w przestrzeni użytkownika (U-mode)
.section .text
.globl _start

_start:
    # Ustawiamy numer wywołania systemowego w a7
    li      a7, 1           # Nasz customowy syscall nr 1 (np. "print hello")
    
    # Wywołanie systemowe - to przerwie wykonywanie i przeniesie nas do jądra (S-mode)
    ecall

    # Jeśli somehow wrócimy z ecall, wpadamy w pętlę
.loop:
    j       .loop
