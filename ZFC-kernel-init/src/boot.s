# boot.s – uruchamiany w trybie M
.section .text.boot
.globl _start

_start:
    # 1. Ustaw tymczasowy stos na czas inicjalizacji w M-mode
    la      sp, _m_stack_top

    # 2. (Opcjonalnie) Wyłącz/odmaskuj przerwania w M-mode na czas konfiguracji
    csrw    mie, zero

    # 3. Skonfiguruj PMP (Physical Memory Protection) – często wymagane,
    #    aby mret nie spowodował illegal instruction w QEMU
    #    (np. zezwól na dostęp do całej przestrzeni adresowej)
    li      t0, -1
    csrw    pmpaddr0, t0
    li      t0, 0x0F          # TOR, RWX dla trybu S/U
    csrw    pmpcfg0, t0

    # 4. Deleguj wyjątki i przerwania do trybu S
    #    (wartości przykładowe – dostosuj do swojej platformy)
    li      t0, 0xb1ff        # medeleg: deleguj m.in. page faulty, ecall z U-mode
    csrw    medeleg, t0
    li      t0, 0x222         # mideleg: deleguj przerwania supervisor (SSI, STI, SEI)
    csrw    mideleg, t0

    # 5. Ustaw wektor przerwań dla trybu M (na wypadek nieoczekiwanych trapów)
    la      t0, m_trap_vector
    csrw    mtvec, t0

    # 6. Przygotuj mstatus: ustaw MPP = S-mode (bits 11-12 = 01)
    #    oraz wyczyść inne niepotrzebne pola
    csrr    t0, mstatus
    li      t1, ~(0x3 << 11)  # maska dla MPP
    and     t0, t0, t1        # wyczyść MPP
    li      t1, (1 << 11)     # MPP = 01 (S-mode)
    or      t0, t0, t1
    csrw    mstatus, t0

    # 7. Ustaw mepc na adres wejścia jądra (init.s / _start_kernel)
    la      t0, _start_kernel
    csrw    mepc, t0

    # 8. Ustaw argumenty dla jądra (zgodnie z konwencją RISC-V)
    #    a0 = hart ID, a1 = adres DTB (Device Tree Blob) – jeśli dostępne
    csrr    a0, mhartid
    # la    a1, _dtb_addr    # odkomentuj, jeśli przekazujesz DTB

    # 9. Wykonaj mret – przejście do trybu S i skok pod mepc
    mret

# Prosty wektor trapów dla M-mode (na razie pętla)
m_trap_vector:
    j       m_trap_vector

# Sekcja stosu dla M-mode
.section .bss
.align 4
_m_stack:
    .space 4096
_m_stack_top:
