#ifndef MEMLAYOUT_H
#define MEMLAYOUT_H

/* QEMU virt devices (physical addresses) */
#define UART0 0x10000000L
#define CLINT 0x2000000L
#define CLINT_MTIMECMP(hartid) (CLINT + 0x4000 + 8 * (hartid))
#define CLINT_MTIME (CLINT + 0xBFF8)
#define PLIC 0x0c000000L
#define PLIC_SENABLE(hart) (PLIC + 0x2080 + (hart)*0x100)
#define PLIC_SPRIORITY(hart) (PLIC + 0x201000 + (hart)*0x2000)
#define PLIC_SCLAIM(hart) (PLIC + 0x201004 + (hart)*0x2000)

#define PGSIZE 4096
#define PGSHIFT 12
#define MAXVA (1L << (9 + 9 + 9 + 12 - 1))
#define TRAMPOLINE (MAXVA - PGSIZE)
#define TRAPFRAME (TRAMPOLINE - PGSIZE)

/*
 * --- BEGIN CoTanOS GPLv3 ---
 * Author: CTRL-F-0rg3
 * Modification of xv6-riscv memlayout.h
 *
 * CoTanOS boots with identity-mapped megapages covering
 * 0x80000000-0x88000000 RWX. Unlike xv6, we do not map kernel
 * stacks below the trampoline at this stage; stacks live at the
 * top of physical DRAM so they are already covered by the
 * identity map. Trampoline/user trapframe support is not used yet.
 */
#define KERNBASE 0x80600000L
#define PHYSTOP 0x88000000L
#define KSTACK(p) (PHYSTOP - ((p)+1) * 2 * PGSIZE)
/* --- END CoTanOS GPLv3 --- */

#endif
