# init.s – pierwszy kod jądra w trybie S
.section .text.init
.globl _start_kernel

_start_kernel:
    csrw    sie, zero               # S-mode: żadnych przerwań na czas init
    mv      s0, a0                  # s0 = hartid (z boot.s)
    bnez    a0, .Lpark              # nie-boot harty parkują NATYCHMIAST
                                    # (nie tykają BSS – brak wyścigu)

    # --- 1. Wyzeruj BSS (tylko hart 0) ---
    la      t0, __bss_start
    la      t1, __bss_end
.Lbss_loop:
    bgeu    t0, t1, .Lbss_done
    sd      zero, 0(t0)
    addi    t0, t0, 8
    j       .Lbss_loop
.Lbss_done:

    # --- 2. Stos jądra ---
    la      sp, _kernel_stack_top

    # --- 3. Centrum globalne (ustawia tp) ---
    li      a0, 0                   # hartid = 0
    li      a1, 0                   # dtb = brak (można podmienić później)
    call    cotan_global_init

    # --- 4. Wczesny scheduler (hart 0 kontynuuje) ---
    li      a0, 0
    call    sched_init

    # --- 5. C-kod jądra ---
    li      a0, 0
    call    kernel_main             # nigdy nie wraca (woła sched_start)

.Lpark:
    wfi
    j       .Lpark
