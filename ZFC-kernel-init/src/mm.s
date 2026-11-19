# =============================================================================
# src/mm.s – Sv39 page tables, identity mapping for kernel
# =============================================================================
.section .text.mm, "ax", @progbits

# --- PTE bits ---
.equ PTE_V,           0x001
.equ PTE_R,           0x002
.equ PTE_W,           0x004
.equ PTE_X,           0x008
.equ PTE_U,           0x010
.equ PTE_G,           0x020
.equ PTE_A,           0x040
.equ PTE_D,           0x080
.equ PTE_LEAF_RW_V,   (PTE_R | PTE_W | PTE_V | PTE_A | PTE_D)
.equ PTE_LEAF_RWX_V,  (PTE_R | PTE_W | PTE_X | PTE_V | PTE_A | PTE_D)
.equ PTE_NONLEAF,     (PTE_V)
.equ SATP_MODE_SV39,  0x8000000000000000

# --- Map constants ---
.equ MMIO_VPN2,       0                   # 0x00000000–0x40000000 (CLINT/PLIC/UART)
.equ DRAM_VPN2,       2                   # 0x80000000–0xC0000000 (DRAM)
.equ DRAM_BASE,       0x80000000
.equ DRAM_SIZE,       0x08000000          # 128 MiB
.equ MEGAPAGE_SIZE,   0x200000

# =============================================================================
# Static page tables – two 4 KiB pages in .bss
# =============================================================================
.section .bss
.balign 4096
.globl g_root_pt
g_root_pt:
.space 4096

.balign 4096
.globl g_l1_dram
g_l1_dram:
.space 4096

# =============================================================================
.section .text.mm, "ax", @progbits

# --- void mm_init(void) ---
# root[0]   = gigapage RW (MMIO 0x00000000–0x40000000)
# root[2]   = -> g_l1_dram (DRAM 0x80000000–0xC0000000)
.globl mm_init
mm_init:
    la      t0, g_root_pt
    la      t1, g_l1_dram
    
    # Zero both tables (2 × 512 × 8 B)
    li      t2, 0
    li      t3, 1024
    mv      t4, t0
.Lmm_zero:
    sd      t2, 0(t4)
    addi    t4, t4, 8
    addi    t3, t3, -1
    bnez    t3, .Lmm_zero
    
    # root[MMIO_VPN2] = gigapage identity map 0x00000000, RW (no X – MMIO)
    li      t3, PTE_LEAF_RW_V
    sd      t3, (MMIO_VPN2 * 8)(t0)
    
    # root[DRAM_VPN2] = non-leaf -> g_l1_dram
    srli    t3, t1, 12
    slli    t3, t3, 10
    ori     t3, t3, PTE_V
    sd      t3, (DRAM_VPN2 * 8)(t0)
    
    ret

# --- void mm_map_megapage(u64 va, u64 pa, u64 flags) ---
# va and pa must be 2 MiB aligned.
# Assumes root[VPN2(va)] already points to L1.
# Disabled: replaced by C implementation in mm_walk.c
mm_map_megapage_asm_old:
    addi    sp, sp, -64
    sd      ra, 0(sp)
    sd      s0, 8(sp)
    sd      s1, 16(sp)
    sd      s2, 24(sp)
    sd      s3, 32(sp)
    sd      s4, 40(sp)

    mv      s0, a0                  # va
    mv      s1, a1                  # pa
    mv      s2, a2                  # flags
    la      s3, g_root_pt           # current table
    li      s4, 2                   # start at level 2

.Lwalk:
    # idx = (va >> (12 + 9*level)) & 0x1FF
    li      t0, 9
    mul     t0, t0, s4
    addi    t0, t0, 12
    srl     t1, s0, t0
    andi    t1, t1, 0x1FF
    slli    t2, t1, 3
    add     t2, s3, t2              # &table[idx]
    ld      t3, 0(t2)
    andi    t4, t3, 1
    bnez    t4, .Lhave_child

    # Slot empty: allocate a child table from the PMM.
    sd      t2, 48(sp)              # save slot across the C call
    call    pmm_alloc_page
    ld      t2, 48(sp)
    beqz    a0, .Lmap_fail
    mv      t5, a0

    mv      t6, t5
    li      t0, 512
.Lzero:
    sd      zero, 0(t6)
    addi    t6, t6, 8
    addi    t0, t0, -1
    bnez    t0, .Lzero

    srli    t0, t5, 12
    slli    t0, t0, 10
    ori     t0, t0, 1               # V only, no U on intermediate entries
    sd      t0, 0(t2)
    mv      t3, t0                  # Update t3 so .Lhave_child uses the new PTE

.Lhave_child:
    srli    t3, t3, 10
    slli    t3, t3, 12
    mv      s3, t3
    addi    s4, s4, -1
    bgtz    s4, .Lwalk

    # Leaf at level 1: idx = (va >> 21) & 0x1FF
    srli    t0, s0, 21
    andi    t0, t0, 0x1FF
    slli    t0, t0, 3
    add     t0, s3, t0
    srli    t1, s1, 12
    slli    t1, t1, 10
    or      t1, t1, s2
    ori     t1, t1, 1               # V
    ori     t1, t1, 0xC0            # A|D pre-set, avoids access faults
    sd      t1, 0(t0)
    li      a0, 0
    j       .Lmap_ret

.Lmap_fail:
    li      a0, -1
.Lmap_ret:
    ld      ra, 0(sp)
    ld      s0, 8(sp)
    ld      s1, 16(sp)
    ld      s2, 24(sp)
    ld      s3, 32(sp)
    ld      s4, 40(sp)
    addi    sp, sp, 64
    ret


.globl mm_map_kernel
mm_map_kernel:
    addi    sp, sp, -48
    sd      ra, 0(sp)
    sd      s0, 8(sp)
    sd      s1, 16(sp)
    sd      s2, 24(sp)
    sd      s3, 32(sp)

    li      s0, DRAM_BASE
    li      s1, DRAM_BASE + DRAM_SIZE
    li      s2, MEGAPAGE_SIZE
    li      s3, PTE_LEAF_RWX_V | PTE_G

.Lmm_kmap_loop:
    bgeu    s0, s1, .Lmm_kmap_done
    mv      a0, s0
    mv      a1, s0
    mv      a2, s3
    call    mm_map_megapage
    add     s0, s0, s2
    j       .Lmm_kmap_loop

.Lmm_kmap_done:
    # Map MMIO devices (CLINT, PLIC, UART) as RW (no X, no U)
    # CLINT: 0x2000000 (64KB) -> map 2MB megapage at 0x2000000
    li      a0, 0x2000000
    li      a1, 0x2000000
    li      a2, PTE_R | PTE_W | PTE_V
    call    mm_map_megapage
    
    # PLIC: 0x0c000000 (4MB) -> map 2MB megapage at 0x0c000000
    li      a0, 0x0c000000
    li      a1, 0x0c000000
    li      a2, PTE_R | PTE_W | PTE_V
    call    mm_map_megapage
    
    # UART0: 0x10000000 (64KB) -> map 2MB megapage at 0x10000000
    li      a0, 0x10000000
    li      a1, 0x10000000
    li      a2, PTE_R | PTE_W | PTE_V
    call    mm_map_megapage
    ld      ra, 0(sp)
    ld      s0, 8(sp)
    ld      s1, 16(sp)
    ld      s2, 24(sp)
    ld      s3, 32(sp)
    addi    sp, sp, 48
    ret




.globl mm_enable_paging
mm_enable_paging:
    la      t0, g_root_pt
    srli    t0, t0, 12
    li      t1, 8
    slli    t1, t1, 60
    or      t0, t0, t1
    csrw    satp, t0
    sfence.vma zero, zero
    ret

# --- void mm_disable_paging(void) --- (debug)
.globl mm_disable_paging
mm_disable_paging:
    sfence.vma zero, zero
    csrw    satp, zero
    sfence.vma zero, zero
    
    ret

