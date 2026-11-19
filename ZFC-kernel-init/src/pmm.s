# =============================================================================
# src/pmm.s – prosty bitmapowy allocator stron fizycznych
# 1 bit = 1 strona 4 KiB; 1 = zajęte, 0 = wolne.
# =============================================================================

.section .text.pmm, "ax", @progbits

.equ PAGE_SHIFT,      12
.equ PAGE_SIZE,       4096
.equ PMM_MAX_PAGES,   32768
.equ PMM_BITMAP_WORDS, (PMM_MAX_PAGES / 64)

# =============================================================================
.section .bss
.balign 8
g_pmm_bitmap:
    .space PMM_BITMAP_WORDS * 8

.globl g_pmm_base
g_pmm_base:
    .space 8

.globl g_pmm_pages
g_pmm_pages:
    .space 8

.globl g_pmm_free
g_pmm_free:
    .space 8

# =============================================================================
.section .text.pmm, "ax", @progbits

# --- void pmm_init(u64 base, u64 size) ---
# base musi być wyrównany do 4 KiB.
.globl pmm_init
pmm_init:
    addi    sp, sp, -16
    sd      s0, 0(sp)
    sd      s1, 8(sp)

    la      t0, g_pmm_base
    sd      a0, 0(t0)

    li      t1, PAGE_SIZE
    divu    s0, a1, t1              # s0 = liczba stron

    la      t0, g_pmm_pages
    sd      s0, 0(t0)
    la      t0, g_pmm_free
    sd      s0, 0(t0)               # wszystkie wolne

    # Wyzeruj bitmapę
    la      t0, g_pmm_bitmap
    li      t2, 0
    li      t3, PMM_BITMAP_WORDS
.Lpmm_zero:
    sd      t2, 0(t0)
    addi    t0, t0, 8
    addi    t3, t3, -1
    bnez    t3, .Lpmm_zero

    # Oznacz jako zajęte bity >= s0 (out-of-range: nie alokowalne)
    mv      t1, s0
    li      t2, PMM_MAX_PAGES
    la      t0, g_pmm_bitmap
.Lpmm_mark_oob:
    bgeu    t1, t2, .Lpmm_init_done
    srli    t3, t1, 6
    andi    t4, t1, 63
    slli    t5, t3, 3
    add     t5, t5, t0
    ld      t6, 0(t5)
    li      t3, 1
    sll     t3, t3, t4
    or      t6, t6, t3
    sd      t6, 0(t5)
    addi    t1, t1, 1
    j       .Lpmm_mark_oob
.Lpmm_init_done:
    ld      s0, 0(sp)
    ld      s1, 8(sp)
    addi    sp, sp, 16
    ret

# --- paddr_t pmm_alloc_page(void) ---
.globl pmm_alloc_page
pmm_alloc_page:
    la      t0, g_pmm_bitmap
    li      t2, 0
    li      s0, PMM_BITMAP_WORDS
.Lpmm_scan_word:
    bgeu    t2, s0, .Lpmm_fail
    slli    t4, t2, 3
    add     t4, t4, t0
    ld      t5, 0(t4)
    not     t5, t5                  # 1 = wolne
    beqz    t5, .Lpmm_next_word

    # Znajdź pierwszą ustawioną 1 – skan bit po bicie (bez Zbb)
    li      t6, 0
.Lpmm_find_bit:
    andi    t4, t5, 1
    bnez    t4, .Lpmm_found
    srli    t5, t5, 1
    addi    t6, t6, 1
    li      t4, 64
    bgeu    t6, t4, .Lpmm_next_word
    j       .Lpmm_find_bit
.Lpmm_found:
    slli    t5, t2, 6
    add     t5, t5, t6              # t5 = page_index

    # Oznacz jako zajęte
    srli    t3, t5, 6
    andi    t4, t5, 63
    slli    t6, t3, 3
    add     t6, t6, t0
    ld      t3, 0(t6)
    li      t2, 1
    sll     t2, t2, t4
    or      t3, t3, t2
    sd      t3, 0(t6)

    # Update free
    la      t3, g_pmm_free
    ld      t4, 0(t3)
    addi    t4, t4, -1
    sd      t4, 0(t3)

    # PA = base + page_index * PAGE_SIZE
    la      t3, g_pmm_base
    ld      t3, 0(t3)
    slli    t4, t5, PAGE_SHIFT
    add     a0, t3, t4
    ret

.Lpmm_next_word:
    addi    t2, t2, 1
    j       .Lpmm_scan_word
.Lpmm_fail:
    li      a0, 0
    ret

# --- void pmm_free_page(paddr_t pa) ---
.globl pmm_free_page
pmm_free_page:
    beqz    a0, .Lpmm_free_ret
    la      t0, g_pmm_base
    ld      t0, 0(t0)
    sub     t1, a0, t0
    srli    t1, t1, PAGE_SHIFT      # page_index
    srli    t3, t1, 6
    andi    t4, t1, 63
    la      t5, g_pmm_bitmap
    slli    t6, t3, 3
    add     t5, t5, t6
    ld      t6, 0(t5)
    li      t3, 1
    sll     t3, t3, t4
    not     t3, t3
    and     t6, t6, t3
    sd      t6, 0(t5)
    la      t3, g_pmm_free
    ld      t4, 0(t3)
    addi    t4, t4, 1
    sd      t4, 0(t3)
.Lpmm_free_ret:
    ret
.Lyield_first:
  call sched_next
  beqz a0, .Lyield_none
  la t0, g_shed
  sd a0, SCHED_CURRENT(t0)
  sd a0, 0x030(tp)
  li t1, TASK_RUNNING
  sd t1, TASJ_STATE(a0)
# --- void pmm_mark_used(paddr_t pa, u64 size) ---
# Idempotentne: nie psuje licznika jeśli strona już zajęta.
.globl pmm_mark_used
pmm_mark_used:
    beqz    a1, .Lpmm_mark_ret
    addi    sp, sp, -16
    sd      s0, 0(sp)
    sd      s1, 8(sp)

    la      t0, g_pmm_base
    ld      t0, 0(t0)
    sub     t1, a0, t0
    srli    s0, t1, PAGE_SHIFT      # start_page

    add     t2, a0, a1
    sub     t2, t2, t0
    addi    t2, t2, PAGE_SIZE - 1
    srli    s1, t2, PAGE_SHIFT      # end_page

.Lpmm_mark_loop:
    bgeu    s0, s1, .Lpmm_mark_done
    srli    t3, s0, 6
    andi    t4, s0, 63
    la      t5, g_pmm_bitmap
    slli    t6, t3, 3
    add     t5, t5, t6
    ld      t6, 0(t5)
    li      t3, 1
    sll     t3, t3, t4
    and     t4, t6, t3              # było już zajęte?
    or      t6, t6, t3
    sd      t6, 0(t5)
    bnez    t4, .Lpmm_mark_skip
    la      t5, g_pmm_free
    ld      t6, 0(t5)
    addi    t6, t6, -1
    sd      t6, 0(t5)
.Lpmm_mark_skip:
    addi    s0, s0, 1
    j       .Lpmm_mark_loop
.Lpmm_mark_done:
    ld      s0, 0(sp)
    ld      s1, 8(sp)
    addi    sp, sp, 16
.Lpmm_mark_ret:
    ret

    ld      t0, 0x038(tp)       # COTAN_OFF_TICKS
    addi    t0, t0, 1
    sd      t0, 0x038(tp)
    call    sched_tick

