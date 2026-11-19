# =============================================================================
# src/global.s – Kernel Control Block (centrum globalne)
# Rejestr tp = &g_cotan od momentu cotan_global_init aż do końca życia jądra.
# =============================================================================

.section .text.global, "ax", @progbits

.equ COTAN_MAGIC,          0x436F54616E4F5331
.equ COTAN_VERSION,        0x0000000100000000

.equ COTAN_OFF_MAGIC,          0x000
.equ COTAN_OFF_VERSION,        0x008
.equ COTAN_OFF_PHASE,          0x010
.equ COTAN_OFF_FLAGS,          0x018
.equ COTAN_OFF_BOOT_HARTID,    0x020
.equ COTAN_OFF_HART_COUNT,     0x028
.equ COTAN_OFF_CURRENT_TASK,   0x030
.equ COTAN_OFF_TICKS,          0x038
.equ COTAN_OFF_SCHED_PTR,      0x080
.equ COTAN_OFF_PMM_BITMAP,     0x088
.equ COTAN_OFF_ROOT_PT,        0x090
.equ COTAN_OFF_TRAP_VEC,       0x098
.equ COTAN_OFF_BOOT_DTB,       0x0A0
.equ COTAN_OFF_HART_DATA,      0x0C0
.equ COTAN_OFF_PANIC_HART,     0x100
.equ COTAN_OFF_PANIC_RA,       0x108
.equ COTAN_OFF_PANIC_PHASE,    0x118
.equ COTAN_OFF_PANIC_MSG,      0x120

.equ COTAN_FLAG_PANIC,         (1 << 0)

# =============================================================================
# --- int cotan_global_init(u64 hart_id, u64 dtb_pa) ---
# Wołane tylko z hartu bootującego. Inne harty muszą być już zaparkowane.
# Ustawia tp = &g_cotan (na stałe do końca życia jądra).
# =============================================================================
.globl cotan_global_init
cotan_global_init:
    bnez    a0, .Lginit_err           # tylko hart 0

    la      t0, g_cotan

    li      t1, COTAN_MAGIC
    sd      t1, COTAN_OFF_MAGIC(t0)

    li      t1, COTAN_VERSION
    sd      t1, COTAN_OFF_VERSION(t0)

    sd      a0, COTAN_OFF_BOOT_HARTID(t0)
    sd      a1, COTAN_OFF_BOOT_DTB(t0)

    li      t1, 1
    sd      t1, COTAN_OFF_HART_COUNT(t0)

    # phase = PHASE_BOOT (już 0 z BSS, ale jawnie)
    sd      zero, COTAN_OFF_PHASE(t0)
    sd      zero, COTAN_OFF_FLAGS(t0)

    # Ustaw tp – od tej chwili każdy może wołać cotan_global() w C
    mv      tp, t0

    li      a0, 0
    ret

.Lginit_err:
    li      a0, -1
    ret

# =============================================================================
# --- void cotan_panic_halt(u64 code, const char *msg) ---
# Nie wraca. Zapisuje diagnostykę i wchodzi w WFI-loop.
# =============================================================================
.globl cotan_panic_halt
cotan_panic_halt:
    sd      a0, COTAN_OFF_PANIC_PHASE(tp)
    csrr    t0, sepc
    sd      t0, COTAN_OFF_PANIC_RA(tp)
    csrr    t0, cycle
    sd      t0, COTAN_OFF_HART_DATA + 0x00(tp)     # placeholder, nieużywane

    # Skopiuj wiadomość (max COTAN_PANIC_MSG_LEN = 64, ze znakiem NUL)
    beqz    a1, .Lpanic_no_msg
    addi    t0, tp, COTAN_OFF_PANIC_MSG
    mv      t1, a1
    li      t2, 63
.Lpanic_copy:
    lbu     t3, 0(t1)
    sb      t3, 0(t0)
    beqz    t3, .Lpanic_copy_done
    addi    t0, t0, 1
    addi    t1, t1, 1
    addi    t2, t2, -1
    bnez    t2, .Lpanic_copy
    sb      zero, 0(t0)
.Lpanic_copy_done:
.Lpanic_no_msg:
    # Set panic flag
    ld      t0, COTAN_OFF_FLAGS(tp)
    li      t1, COTAN_FLAG_PANIC
    or      t0, t0, t1
    sd      t0, COTAN_OFF_FLAGS(tp)
.Lpanic_hang:
    wfi
    j       .Lpanic_hang

# =============================================================================
# Zmienne w .bss – 64B-aligned, żeby trafiły na własne linie cache
# =============================================================================
.section .bss
.balign 64
.globl g_cotan
g_cotan:
    .space 0x200                # 512 B – sizeof(struct cotan_global)

.balign 64
.globl g_hart_data
g_hart_data:
    .space 0x40 * 8             # 8 hartów × 64 B
