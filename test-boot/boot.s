.global _start

.section .text
_start:
    # Ustawienie stosu (wskazujemy na prosty obszar pamięci RAM)
    la sp, stack_top

    # Wypisanie znaku 'O' przez rejestr debugowania/UART QEMU (NS16550A / sifive_test)
    # W maszynie 'virt' QEMU standardowy UART znajduje się pod adresem 0x10000000
    li t0, 0x10000000
    li t1, 'C'
    sb t1, 0(t0)
    li t1, 'o'
    sb t1, 0(t0)
    li t1, 'T'
    sb t1, 0(t0)
    li t1, 'a'
    sb t1, 0(t0)
    li t1, 'n'
    sb t1, 0(t0)
    li t1, 'O'
    sb t1, 0(t0)
    li t1, 'S'
    sb t1, 0(t0)
    li t1, '\n'
    sb t1, 0(t0)

hang:
    wfi
    j hang

.section .bss
.space 4096
stack_top: