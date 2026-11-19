#ifndef _COTAN_MM_H
#define _COTAN_MM_H

#include "CoTan_types.h"

/* ---- geometria ---- */
#define PAGE_SHIFT 12
#define PAGE_SIZE (1UL << PAGE_SHIFT) /* 4 KiB */
#define PAGE_MASK (~(PAGE_SIZE - 1))
#define MEGAPAGE_SHIFT 21
#define MEGAPAGE_SIZE (1UL << MEGAPAGE_SHIFT) /* 2 MiB */
#define MEGAPAGE_MASK (~(MEGAPAGE_SIZE - 1))
#define GIGAPAGE_SHIFT 30
#define GIGAPAGE_SIZE (1UL << GIGAPAGE_SHIFT) /* 1 GiB */

/* ---- Sv39 satp ---- */
#define SATP_MODE_SV39 (8UL << 60)
#define SATP_MODE_MASK (0xFUL << 60)
#define SATP_ASID_SHIFT 44
#define SATP_PPN_MASK 0xFFFFFFFFFFFUL

/* ---- PTE bity (RISC-V Sv39) ---- */
#define PTE_V (1UL << 0)
#define PTE_R (1UL << 1)
#define PTE_W (1UL << 2)
#define PTE_X (1UL << 3)
#define PTE_U (1UL << 4)
#define PTE_G (1UL << 5)
#define PTE_A (1UL << 6)
#define PTE_D (1UL << 7)

/* ---- kombinacje ---- */
#define PTE_LEAF_RO_V (PTE_R | PTE_V | PTE_A)
#define PTE_LEAF_RW_V (PTE_R | PTE_W | PTE_V | PTE_A | PTE_D)
#define PTE_LEAF_RWX_V (PTE_R | PTE_W | PTE_X | PTE_V | PTE_A | PTE_D)
#define PTE_NONLEAF (PTE_V) /* wskaźnik do niższego poziomu */

/* ---- QEMU virt: mapa fizyczna ---- */
#define DRAM_BASE 0x80000000UL
#define DRAM_SIZE 0x08000000UL /* 128 MiB */
#define UART_BASE 0x10000000UL
#define PLIC_BASE 0x0c000000UL
#define CLINT_BASE 0x02000000UL

/* ---- pomocnicze makra ---- */
#define VA_VPN2(va) (((u64)(va) >> 30) & 0x1FF)
#define VA_VPN1(va) (((u64)(va) >> 21) & 0x1FF)
#define VA_VPN0(va) (((u64)(va) >> 12) & 0x1FF)
#define PA_TO_PPN(pa) ((u64)(pa) >> 12)
#define PPN_TO_PA(ppn) ((u64)(ppn) << 12)
#define PTE_PPN_PART(pa) ((PA_TO_PPN(pa) & SATP_PPN_MASK) << 10)

/* ---- API ---- */
void mm_init(void);
void mm_map_megapage(u64 va, u64 pa, u64 flags);
void mm_map_kernel(void);
void mm_enable_paging(void);
void mm_disable_paging(void);

#endif
