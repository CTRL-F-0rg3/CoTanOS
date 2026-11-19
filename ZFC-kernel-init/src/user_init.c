/*
 * user_init.c - first U-mode process with its own Sv39 page table.
 * CoTanOS GPLv3
 */
#include "types.h"
#include "param.h"
#include "riscv.h"
#include "proc.h"
#include "memlayout.h"
#include "string.h"

#include "CoTan_uart.h"
#include "CoTan_pmm.h"

/* Kernel root page table, defined in mm.s (identity megapages, S-only). */
extern u64 g_root_pt[];

extern char _binary_ZFC_kernel_init_build_user_test_bin_start[];
extern char _binary_ZFC_kernel_init_build_user_test_bin_end[];

#define UTEXT_VA 0x40000000UL
#define USTACK_VA 0x40001000UL


/* Map one 4K page. Intermediate entries carry V only (no U). */
static int map_user_page(u64 *root, u64 va, u64 pa, u64 flags) {
    u64 *tbl = root;
    for (int level = 2; level > 0; level--) {
        u64 idx = (va >> (12 + 9 * level)) & 0x1FF;
        u64 pte = tbl[idx];
        if (!(pte & PTE_V)) {
            u64 child = (u64)pmm_alloc_page();
            if (!child) return -1;
            memset((void *)child, 0, PGSIZE);
            tbl[idx] = ((child >> 12) << 10) | PTE_V;
            pte = tbl[idx];
        }
        tbl = (u64 *)(((pte >> 10) << 12));
    }
    u64 idx = (va >> 12) & 0x1FF;
    tbl[idx] = ((pa >> 12) << 10) | flags;
    return 0;
}

void start_user_process(void) {
    uart_puts("[user] building address space\n");

    u64 root_pa = (u64)pmm_alloc_page();
    if (!root_pa) { uart_puts("[user] PANIC: no page for root PT\n"); return; }
    memset((void *)root_pa, 0, PGSIZE);
    u64 *root = (u64 *)root_pa;

    /*
     * Copy kernel/MMIO mappings (S-only) so traps and UART keep working
     * after we switch satp. User code cannot touch them: no PTE_U.
     */
    for (int i = 0; i < 512; i++) root[i] = g_root_pt[i];

    u64 code_pa = (u64)pmm_alloc_page();
    u64 stack_pa = (u64)pmm_alloc_page();
    u64 tf_pa = (u64)pmm_alloc_page();
    if (!code_pa || !stack_pa || !tf_pa) {
        uart_puts("[user] PANIC: out of physical pages\n");
        uart_puts("[user] code="); uart_hex(code_pa);
        uart_puts(" stack="); uart_hex(stack_pa);
        uart_puts(" tf="); uart_hex(tf_pa); uart_putc('\n');
        return;
    }
    memset((void *)code_pa, 0, PGSIZE);
    memset((void *)stack_pa, 0, PGSIZE);
    memset((void *)tf_pa, 0, PGSIZE);

    u64 size = (u64)(_binary_ZFC_kernel_init_build_user_test_bin_end -
                     _binary_ZFC_kernel_init_build_user_test_bin_start);
    if (size > PGSIZE) size = PGSIZE;
    memcpy((void *)code_pa, _binary_ZFC_kernel_init_build_user_test_bin_start, size);

    if (map_user_page(root, UTEXT_VA, code_pa,
                      (0x001 | 0x002 | 0x008 | 0x010 | 0x040 | 0x080)) < 0 ||
        map_user_page(root, USTACK_VA, stack_pa,
                      (0x001 | 0x002 | 0x004 | 0x010 | 0x040 | 0x080)) < 0) {
        uart_puts("[user] PANIC: page table walk failed\n");
        return;
    }

    struct trapframe *tf = (struct trapframe *)tf_pa;
    tf->epc = UTEXT_VA;
    tf->sp  = USTACK_VA + PGSIZE;   /* stack grows down */

    uart_puts("[user] text_va="); uart_hex(UTEXT_VA);
    uart_puts(" stack_va="); uart_hex(tf->sp);
    uart_puts(" root="); uart_hex(root_pa); uart_putc('\n');
    uart_puts("[user] entering U-mode via sret\n");

    extern void user_enter(struct trapframe *tf, u64 root_pa);
    user_enter(tf, root_pa);

    uart_puts("[user] ERROR: returned from U-mode\n");
}
