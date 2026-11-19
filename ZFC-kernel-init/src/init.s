# init.S - S-mode entry from OpenSBI. Boot hart is NOT guaranteed to be 0.
.section .data
.align 3
.globl g_boot_elect
g_boot_elect:
    .word 0
.globl g_init_done
g_init_done:
    .word 0

.section .text.init
.globl _start_kernel
_start_kernel:
    csrw    sie, zero
    mv      s0, a0                  # s0 = hartid

    # Elect exactly one primary hart. .data is loaded by the ELF loader,
    # so this flag is valid before BSS is cleared.
    la      t0, g_boot_elect
    li      t1, 1
    amoswap.w.aq t2, t1, (t0)
    bnez    t2, .Lsecondary_wait

.Lprimary:
    la      t0, __bss_start
    la      t1, __bss_end
.Lbss_loop:
    bgeu    t0, t1, .Lbss_done
    sd      zero, 0(t0)
    addi    t0, t0, 8
    j       .Lbss_loop
.Lbss_done:
    la      sp, _kernel_stack_top
    li      a0, 0                   # cotan_global_init requires hart 0
    li      a1, 0                   # dtb (unused for now)
    call    cotan_global_init
    
    # Store real boot hartid in g_cotan.boot_hartid (offset 0x020)
    la      t0, g_cotan
    sd      s0, 0x020(t0)           # COTAN_OFF_BOOT_HARTID
    
    mv      a0, s0                  # real boot hartid for kernel_main
    call    kernel_main
.Lpark:
    wfi
    j       .Lpark

.Lsecondary_wait:
    # Spin on a .data flag using registers only. Do NOT touch BSS here:
    # the primary hart is still clearing it.
    la      t0, g_init_done
.Lwait_loop:
    lw      t1, 0(t0)
    beqz    t1, .Lwait_loop
    fence   r, rw

    # Per-hart stack: __hart_stacks_start + (hartid + 1) * 4096
    la      t0, __hart_stacks_start
    li      t1, 0x1000
    addi    t2, s0, 1
    mul     t2, t1, t2
    add     sp, t0, t2

    mv      a0, s0
    call    secondary_hart_init
    j       .Lpark
