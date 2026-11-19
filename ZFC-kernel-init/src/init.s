# init.s – pierwszy kod jądra w trybie S (OpenSBI entry)
.section .text.init
.globl _start_kernel

_start_kernel:
    csrw    sie, zero               # S-mode: żadnych przerwań na czas init
    mv      s0, a0                  # s0 = hartid (z OpenSBI/DTB)
    
    # Hart 0 robi pełną inicjalizację, inne harty przeskakują do .Lsecondary_init
    bnez    s0, .Lsecondary_init

# =============================================================================
# HART 0 (Boot CPU)
# =============================================================================
.Lprimary_init:
    # 1. Wyzeruj BSS (tylko hart 0)
    la      t0, __bss_start
    la      t1, __bss_end
.Lbss_loop:
    bgeu    t0, t1, .Lbss_done
    sd      zero, 0(t0)
    addi    t0, t0, 8
    j       .Lbss_loop
.Lbss_done:

    # 2. Stos jądra dla hartu 0
    la      sp, _kernel_stack_top

    # 3. Centrum globalne (ustawia tp)
    li      a0, 0                   # hartid = 0
    li      a1, 0                   # dtb = 0 (na razie)
    call    cotan_global_init

    # 4. C-kod jądra (PMM, VM, Scheduler start)
    li      a0, 0
    call    kernel_main             # nigdy nie wraca (woła sched_start)

# =============================================================================
# HART 1+ (Secondary CPUs)
# =============================================================================
.Lsecondary_init:
    # DEBUG: Natychmiastowy marker, że Hart 1 się obudził
    li      t0, 0x10000000        # UART0 base
    li      t1, '1'               # znak '1'
    li      t2, 'B'               # znak 'B' (jak Boot)
    sb      t1, 0(t0)             # wypisz '1'
    sb      t2, 0(t0)             # wypisz 'B'
    
    # 1. Ustaw stos dla tego hartu (używamy __hart_stacks_start z link.ld)
    # Każdy hart dostaje 4KB (0x1000)
    la      t0, __hart_stacks_start
    li      t1, 0x1000
    mul     t1, t1, s0              # offset = hartid * 4096
    add     t0, t0, t1              # t0 = &__hart_stacks_start[hartid * 4096]
    add     sp, t0, t1              # sp = t0 + 4096 (szczyt stosu)

    # 2. Wywołaj inicjalizację drugoplanową
    mv      a0, s0                  # przekaż hartid
    call    secondary_hart_init

    # 3. Jeśli z jakiegoś powodu wróci, zaparkuj
.Lpark:
    wfi
    j       .Lpark
