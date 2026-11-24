#include "types.h"
#include "riscv.h"
#include "param.h"
#include "memlayout.h"
#include "proc.h"
#include "string.h"
#include "xv6_compat.h"

/* CoTanOS headers */
#include "CoTan_pmm.h"
#include "CoTan_uart.h"

/*
 * --- BEGIN CoTanOS GPLv3 ---
 * Author: CTRL-F-0rg3
 * Description: Adapter layer mapping xv6 scheduler dependencies to CoTanOS
 *              PMM and UART, and stubbing out user-space/FS features.
 */

/* --- Memory Allocation --- */
void* kalloc(void) {
    return (void*)pmm_alloc_page();
}

void kfree(void *pa) {
    if (pa) pmm_free_page((paddr_t)pa);
}

/* --- Console --- */
void consputc(int c) {
    uart_putc((char)c);
}

/* --- Virtual Memory --- */
/* We use identity mapping, so these are mostly no-ops or return dummy values */
void kvmmap(pagetable_t kpgtbl, uint64 va, uint64 pa, uint64 sz, int perm) {
    (void)kpgtbl; (void)va; (void)pa; (void)sz; (void)perm;
}

int mappages(pagetable_t pagetable, uint64 va, uint64 size, uint64 pa, int perm) {
    (void)pagetable; (void)va; (void)size; (void)pa; (void)perm;
    return 0; /* success */
}

char trampoline[4096]; /* dummy */

void userret(uint64 pagetable) {
    (void)pagetable;
    /* Should not be reached for kernel threads */
    for(;;);
}

void prepare_return(void) {}

pagetable_t uvmcreate(void) {
    /* Allocate a real page to act as a dummy root page table
     * so xv6's NULL checks in allocproc don't fail. */
    return (pagetable_t)pmm_alloc_page();
}

void uvmfree(pagetable_t pagetable, uint64 sz) {
    (void)sz;
    if (pagetable) kfree((void*)pagetable);
}

pagetable_t uvmcopy(pagetable_t old, pagetable_t new, uint64 sz) {
    (void)old; (void)new; (void)sz;
    return 0;
}

void uvmunmap(pagetable_t pagetable, uint64 va, uint64 npages, int do_free) {
    (void)pagetable; (void)va; (void)npages; (void)do_free;
}

uint64 uvmalloc(pagetable_t pagetable, uint64 oldsz, uint64 newsz) {
    (void)pagetable; (void)oldsz;
    return newsz;
}

uint64 uvmdealloc(pagetable_t pagetable, uint64 oldsz, uint64 newsz) {
    (void)pagetable; (void)oldsz;
    return newsz;
}

/* --- User/Kernel Copy --- */
int copyin(pagetable_t pagetable, char *dst, uint64 srcva, uint64 len) {
    (void)pagetable;
    /* For kernel threads, srcva is just a kernel pointer */
    memmove(dst, (void*)srcva, len);
    return 0;
}

int copyout(pagetable_t pagetable, uint64 dstva, char *src, uint64 len) {
    (void)pagetable;
    memmove((void*)dstva, src, len);
    return 0;
}

/* --- Filesystem / Files --- */
void begin_op(void) {}
void end_op(void) {}
void fsinit(int dev) { (void)dev; }
struct inode* namei(char *path) { (void)path; return 0; }
void iput(struct inode *ip) { (void)ip; }
struct inode* idup(struct inode *ip) { return ip; }
void fileclose(struct file *f) { (void)f; }
struct file* filedup(struct file *f) { return f; }

/* --- Execution --- */
int kexec(char *path, char **argv) {
    (void)path; (void)argv;
    return -1; /* not supported */
}

/* --- END CoTanOS GPLv3 --- */
