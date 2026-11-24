# =============================================================================
# src/sched.s – wczesny, jednordzeniowy scheduler „strażnik”
#
# Zasady:
#   - Tylko hart 0 kontynuuje; pozostałe wołają hart_park().
#   - Kolejka FIFO; brak priorytetów (pełny scheduler później).
#   - Współpracujące (cooperative) + opcjonalne wywłaszczanie z timera.
#   - Wymaga rozszerzenia M (instrukcja mul) LUB linkowania libgcc __muldi3.
#
# Offsety struktur: MUSZĄ być zgodne z include/task.h, queue.h, sched.h
# =============================================================================

.section .text.sched, "ax", @progbits

# --- offsety z task.h ---
.equ TASK_RA,       0x00
.equ TASK_SP,       0x08
.equ TASK_S0,       0x10
.equ TASK_S1,       0x18
.equ TASK_S2,       0x20
.equ TASK_S3,       0x28
.equ TASK_S4,       0x30
.equ TASK_S5,       0x38
.equ TASK_S6,       0x40
.equ TASK_S7,       0x48
.equ TASK_S8,       0x50
.equ TASK_S9,       0x58
.equ TASK_S10,      0x60
.equ TASK_S11,      0x68
.equ TASK_ID,       0x70
.equ TASK_STATE,    0x78
.equ TASK_ENTRY,    0x80
.equ TASK_STACK,    0x88
.equ TASK_TICKS,    0x90
.equ TASK_NEXT,     0x98
.equ TASK_SIZE,     0xA0

# --- stany zadania ---
.equ TASK_UNUSED,   0
.equ TASK_READY,    1
.equ TASK_RUNNING,  2
.equ TASK_BLOCKED,  3
.equ TASK_DONE,     4

# --- offsety z queue.h ---
.equ RQ_HEAD,       0x00
.equ RQ_TAIL,       0x08
.equ RQ_COUNT,      0x10

# --- offsety z sched.h ---
.equ SCHED_CURRENT,   0x00
.equ SCHED_RQ,        0x08
.equ SCHED_BOOT_HART, 0x28
.equ SCHED_NEXT_ID,   0x30
.equ SCHED_PREEMPT,   0x38

.equ MAX_TASKS,       16
.equ TASK_STACK_SIZE, 0x1000

# =============================================================================
# Zmienne globalne (w .bss)
# =============================================================================
.section .bss
.align 3
.globl g_sched
g_sched:
    .space 0x40                     # sizeof(struct scheduler)

.globl g_tasks
g_tasks:
    .space TASK_SIZE * MAX_TASKS

# Stosy per-zadanie. Nie muszą być w .bss – linker i tak je zaalokuje.
# Wystawiamy symbol, żeby linker script mógł je policzyć, jeśli chce.
.globl g_stacks
g_stacks:
    .space TASK_STACK_SIZE * MAX_TASKS

# =============================================================================
# sched_init(uint64_t hart_id)
# a0 = hart_id (z boot.s / OpenSBI)
# Hart 0: inicjuje struktury i wraca.
# Inne harty: wołają hart_park() i już nigdy nie wracają.
# =============================================================================
.section .text.sched, "ax", @progbits
.globl sched_init
sched_init:
    mv      t0, a0                  # t0 = hart_id
    bnez    t0, .Linit_park         # tylko hart 0

    la      t1, g_sched
    sd      t0, SCHED_BOOT_HART(t1) # zapisz boot_hart = 0

    # Wyzeruj runqueue
    sd      zero, SCHED_RQ + RQ_HEAD(t1)
    sd      zero, SCHED_RQ + RQ_TAIL(t1)
    sd      zero, SCHED_RQ + RQ_COUNT(t1)

    # Wyzeruj pozostałe pola schedulera
    sd      zero, SCHED_CURRENT(t1)
    sd      zero, SCHED_PREEMPT(t1)
    li      t2, 1
    sd      t2, SCHED_NEXT_ID(t1)

    ret

.Linit_park:
    tail    hart_park

# =============================================================================
# struct task *sched_create(void (*entry)(void))
# a0 = wskaźnik na funkcję wejściową zadania
# Zwraca: a0 = wskaźnik na nowe zadanie albo NULL.
# =============================================================================
.globl sched_create
sched_create:
    mv      t0, a0                  # t0 = entry
    la      t1, g_tasks
    la      t2, g_sched
    ld      t3, SCHED_NEXT_ID(t2)   # t3 = id startowe
    li      t4, MAX_TASKS
    li      t5, 0                   # licznik iteracji

.Lcreate_find:
    beq     t5, t4, .Lcreate_fail

    # a1 = &g_tasks[t3] = g_tasks + id * TASK_SIZE
    li      a1, TASK_SIZE
    mul     a1, t3, a1
    add     a1, a1, t1

    ld      a2, TASK_STATE(a1)
    beqz    a2, .Lcreate_found

    addi    t3, t3, 1
    blt     t3, t4, .Lcreate_nowrap
    li      t3, 0
.Lcreate_nowrap:
    addi    t5, t5, 1
    j       .Lcreate_find

.Lcreate_found:
    # Wyzeruj strukturę zadania (TASK_SIZE/8 = 20 słów)
    li      a2, 0
    li      a3, TASK_SIZE / 8
    mv      a4, a1
.Lcreate_zero:
    sd      a2, 0(a4)
    addi    a4, a4, 8
    addi    a3, a3, -1
    bnez    a3, .Lcreate_zero

    # id, stan, entry
    sd      t3, TASK_ID(a1)
    li      a2, TASK_UNUSED         # enqueue zmieni na READY
    sd      a2, TASK_STATE(a1)
    sd      t0, TASK_ENTRY(a1)

    # Stos: g_stacks + id * TASK_STACK_SIZE
    li      a2, TASK_STACK_SIZE
    mul     a2, t3, a2
    la      a3, g_stacks
    add     a3, a3, a2              # a3 = stack base
    sd      a3, TASK_STACK(a1)
    li      a4, TASK_STACK_SIZE
    add     a3, a3, a4              # a3 = stack top
    sd      a3, TASK_SP(a1)

    # ra = task_trampoline (punkt wejścia po pierwszym context_switch)
    la      a4, task_trampoline
    sd      a4, TASK_RA(a1)

    # Aktualizacja next_id (round-robin po slotach)
    addi    t3, t3, 1
    blt     t3, t4, .Lcreate_wrap2
    li      t3, 0
.Lcreate_wrap2:
    sd      t3, SCHED_NEXT_ID(t2)

    mv      a0, a1
    ret

.Lcreate_fail:
    li      a0, 0
    ret

# =============================================================================
# void sched_enqueue(struct task *t)
# Dodaje zadanie na koniec runqueue i ustawia stan READY.
# =============================================================================
.globl sched_enqueue
sched_enqueue:
    sd      zero, TASK_NEXT(a0)
    li      t0, TASK_READY
    sd      t0, TASK_STATE(a0)

    la      t0, g_sched + SCHED_RQ
    ld      t1, RQ_TAIL(t0)

    beqz    t1, .Lenq_empty
    # queue niepusta: dopisz na koniec
    sd      a0, TASK_NEXT(t1)
    sd      a0, RQ_TAIL(t0)
    j       .Lenq_bump

.Lenq_empty:
    sd      a0, RQ_HEAD(t0)
    sd      a0, RQ_TAIL(t0)

.Lenq_bump:
    ld      t1, RQ_COUNT(t0)
    addi    t1, t1, 1
    sd      t1, RQ_COUNT(t0)
    ret

# =============================================================================
# struct task *sched_next(void)  – wewnętrzne
# Zdejmuje zadanie z czoła runqueue (albo NULL, jeśli pusta).
# =============================================================================
.globl sched_next
sched_next:
    la      t0, g_sched + SCHED_RQ
    ld      t1, RQ_HEAD(t0)
    beqz    t1, .Lnext_none

    ld      t2, TASK_NEXT(t1)
    sd      t2, RQ_HEAD(t0)

    bnez    t2, .Lnext_tail_ok
    sd      zero, RQ_TAIL(t0)
.Lnext_tail_ok:
    ld      t3, RQ_COUNT(t0)
    addi    t3, t3, -1
    sd      t3, RQ_COUNT(t0)

    sd      zero, TASK_NEXT(t1)
    mv      a0, t1
    ret

.Lnext_none:
    li      a0, 0
    ret

# =============================================================================
# void context_switch(struct task *prev, struct task *next)  – wewnętrzne
# Zapisuje kontekst prev (jeśli != NULL) i ładuje kontekst next.
# =============================================================================
.globl context_switch
context_switch:
    beqz    a0, .Lsw_restore       # brak prev – pierwszy switch

    sd      ra, TASK_RA(a0)
    sd      sp, TASK_SP(a0)
    sd      s0, TASK_S0(a0)
    sd      s1, TASK_S1(a0)
    sd      s2, TASK_S2(a0)
    sd      s3, TASK_S3(a0)
    sd      s4, TASK_S4(a0)
    sd      s5, TASK_S5(a0)
    sd      s6, TASK_S6(a0)
    sd      s7, TASK_S7(a0)
    sd      s8, TASK_S8(a0)
    sd      s9, TASK_S9(a0)
    sd      s10, TASK_S10(a0)
    sd      s11, TASK_S11(a0)

.Lsw_restore:
    ld      ra, TASK_RA(a1)
    ld      sp, TASK_SP(a1)
    ld      s0, TASK_S0(a1)
    ld      s1, TASK_S1(a1)
    ld      s2, TASK_S2(a1)
    ld      s3, TASK_S3(a1)
    ld      s4, TASK_S4(a1)
    ld      s5, TASK_S5(a1)
    ld      s6, TASK_S6(a1)
    ld      s7, TASK_S7(a1)
    ld      s8, TASK_S8(a1)
    ld      s9, TASK_S9(a1)
    ld      s10, TASK_S10(a1)
    ld      s11, TASK_S11(a1)
    ret

# =============================================================================
# task_trampoline – pierwszy kod nowego zadania
# Wykonywany po pierwszym context_switch: ra = trampoline, sp = stack top.
# Wywołuje entry(); po powrocie oznacza zadanie DONE i wchodzi w pętlę yield.
# =============================================================================
.globl task_trampoline
task_trampoline:
    la      t0, g_sched
    ld      t0, SCHED_CURRENT(t0)
    ld      t1, TASK_ENTRY(t0)
    jalr    t1                      # entry() – jeśli wróci, lecimy dalej

    # zadanie wróciło – oznacz DONE i wywołuj yield w pętli (nic już nie robi)
    la      t0, g_sched
    ld      t0, SCHED_CURRENT(t0)
    li      t1, TASK_DONE
    sd      t1, TASK_STATE(t0)
.Ltramp_loop:
    call    sched_yield
    j       .Ltramp_loop

# =============================================================================
# void sched_yield(void)
# Współpracujące wywłaszczenie – oddaje CPU innemu READY.
# Zachowuje ra, s0, s1 (callee-saved). t/a są caller-saved (możemy je niszczyć).
# =============================================================================
.globl sched_yield
sched_yield:
    addi    sp, sp, -32
    sd      ra, 0(sp)
    sd      s0, 8(sp)
    sd      s1, 16(sp)
    # slot 24(sp) rezerwowy dla przyszłej rozbudowy

    la      s0, g_sched
    ld      s1, SCHED_CURRENT(s0)   # s1 = prev

    beqz    s1, .Lyield_first

    # prev != NULL – oznacz READY i wstaw z powrotem do queue,
    # chyba że jest już DONE.
    ld      t0, TASK_STATE(s1)
    li      t1, TASK_DONE
    beq     t0, t1, .Lyield_first

    li      t0, TASK_READY
    sd      t0, TASK_STATE(s1)
    mv      a0, s1
    call    sched_enqueue

.Lyield_first:
    call    sched_next              # a0 = next (lub NULL)
    beqz    a0, .Lyield_none

    # mamy next – ustaw current i zrób context_switch(prev, next)
    la      t0, g_sched
    sd      a0, SCHED_CURRENT(t0)
    li      t1, TASK_RUNNING
    sd      t1, TASK_STATE(a0)

    mv      a1, a0
    mv      a0, s1
    call    context_switch
    j       .Lyield_done

.Lyield_none:
    # Brak READY – jeśli current nie jest DONE, kontynuuj go.
    ld      t0, SCHED_CURRENT(s0)
    beqz    t0, .Lyield_halt
    ld      t1, TASK_STATE(t0)
    li      t2, TASK_DONE
    beq     t1, t2, .Lyield_halt
    j       .Lyield_done

.Lyield_halt:
    # Brak sensownej roboty – wyczyść current i uśpij hart.
    la      t0, g_sched
    sd      zero, SCHED_CURRENT(t0)
.Lyield_halt_loop:
    wfi
    j       .Lyield_halt_loop

.Lyield_done:
    ld      ra, 0(sp)
    ld      s0, 8(sp)
    ld      s1, 16(sp)
    addi    sp, sp, 32
    ret

# =============================================================================
# void sched_start(void) – uruchamia scheduler, nigdy nie wraca do wołającego.
# =============================================================================
.globl sched_start
sched_start:
    # Włącz wywłaszczanie z timera (opcjonalne – jeśli sched_tick podpięty)
    la      t0, g_sched
    li      t1, 1
    sd      t1, SCHED_PREEMPT(t0)
.Lstart_loop:
    call    sched_yield
    j       .Lstart_loop

# =============================================================================
# void sched_tick(void) – wywoływane z handlera timera.
# Jeśli preempt_en == 1, wymusza yield (round-robin po ticku).
# =============================================================================
.globl sched_tick
sched_tick:
    la      t0, g_sched
    ld      t1, SCHED_PREEMPT(t0)
    beqz    t1, .Ltick_ret

    # Zwiększ licznik ticków bieżącego zadania
    ld      t1, SCHED_CURRENT(t0)
    beqz    t1, .Ltick_yield
    ld      t2, TASK_TICKS(t1)
    addi    t2, t2, 1
    sd      t2, TASK_TICKS(t1)

.Ltick_yield:
    tail    sched_yield

.Ltick_ret:
    ret

# =============================================================================
# void hart_park(void) – dla hartów != 0: wyłącz przerwania i śpij.
# =============================================================================
.globl hart_park
hart_park:
    csrw    sie, zero               # S-mode: żadnych przerwań
    csrw    sip, zero               # wyczyść pending
    csrci   sstatus, 0x2            # wyczyść SIE (bit 1)
.Lpark_loop:
    wfi
    j       .Lpark_loop
