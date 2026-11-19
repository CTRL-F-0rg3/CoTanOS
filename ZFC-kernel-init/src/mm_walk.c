/*
 * mm_walk.c - Sv39 2MB megapage mapper with intermediate table allocation.
 * CoTanOS GPLv3
 *
 * Called before paging is enabled, so VA == PA (identity) here.
 */
#include "CoTan_types.h"
#include "CoTan_uart.h"
#include "CoTan_pmm.h"

extern u64 g_root_pt[];

#define PTE_V 0x001UL
#define PTE_A 0x040UL
#define PTE_D 0x080UL

#define PTE_PA(pte) (((pte) >> 10) << 12)
#define PA_PTE(pa)  (((pa) >> 12) << 10)

/* Set to 1 to trace every table allocation. */
static int mm_walk_debug = 1;

int mm_map_megapage(u64 va, u64 pa, u64 flags) {
    if (va & 0x1FFFFFUL) {
        uart_puts("[mm] misaligned megapage va="); uart_hex(va); uart_putc('\n');
        return -1;
    }

    u64 *table = g_root_pt;

    /* Descend L2 -> L1. The leaf is a 2MB entry placed in the L1 table. */
    for (int level = 2; level > 1; level--) {
        u64 idx = (va >> (12 + 9 * level)) & 0x1FFUL;
        u64 pte = table[idx];

        if (!(pte & PTE_V)) {
            u64 child = (u64)pmm_alloc_page();
            if (!child) {
                uart_puts("[mm] pmm_alloc_page failed, va="); uart_hex(va); uart_putc('\n');
                return -1;
            }
            if (child & 0xFFFUL) {
                uart_puts("[mm] PMM returned unaligned page: "); uart_hex(child); uart_putc('\n');
                return -1;
            }

            volatile u64 *z = (volatile u64 *)child;
            for (int i = 0; i < 512; i++) z[i] = 0;

            /* V only: never PTE_U on intermediate entries. */
            table[idx] = PA_PTE(child) | PTE_V;
            pte = table[idx];

            if (mm_walk_debug) {
                uart_puts("[mm] L"); uart_putc('0' + level);
                uart_puts(" alloc idx="); uart_hex(idx);
                uart_puts(" tbl="); uart_hex(child); uart_putc('\n');
            }
        }

        u64 next = PTE_PA(pte);
        if (!next) {
            uart_puts("[mm] zero child PA, pte="); uart_hex(pte); uart_putc('\n');
            return -1;
        }
        table = (u64 *)next;
    }

    u64 idx = (va >> 21) & 0x1FFUL;
    table[idx] = PA_PTE(pa) | flags | PTE_V | PTE_A | PTE_D;
    return 0;
}
