#ifndef _COTAN_PMM_H
#define _COTAN_PMM_H

#include "CoTan_types.h"

#define PMM_MAX_PAGES 32768                   /* 128 MiB / 4 KiB */
#define PMM_BITMAP_WORDS (PMM_MAX_PAGES / 64) /* 512 */

/* Statystyki / stan (w .bss) */
extern u64 g_pmm_base;
extern u64 g_pmm_pages;
extern u64 g_pmm_free;

/* API */
void pmm_init(paddr_t base, u64 size);
paddr_t pmm_alloc_page(void);
void pmm_free_page(paddr_t pa);
void pmm_mark_used(paddr_t pa, u64 size);

#endif
