#ifndef XV6_COMPAT_H
#define XV6_COMPAT_H

/* MUST be first: defines uint64, uint, etc. */
#include "types.h"
#include "riscv.h"
#include "fs.h"
#include "file.h"

/* Filesystem stubs */
void begin_op(void);
void end_op(void);
void fsinit(int dev);
struct inode* namei(char *path);
void iput(struct inode *ip);
struct inode* idup(struct inode *ip);

/* File stubs */
void fileclose(struct file *f);
struct file* filedup(struct file *f);

/* Console */
void consputc(int c);

/* Memory copy */
int copyin(pagetable_t pagetable, char *dst, uint64 srcva, uint64 len);
int copyout(pagetable_t pagetable, uint64 dstva, char *src, uint64 len);

/* Execution */
int kexec(char *path, char **argv);

/* Virtual memory */
void kvmmap(pagetable_t kpgtbl, uint64 va, uint64 pa, uint64 sz, int perm);
int mappages(pagetable_t pagetable, uint64 va, uint64 size, uint64 pa, int perm);
extern char trampoline[];
void userret(uint64 pagetable);
void prepare_return(void);

pagetable_t uvmcreate(void);
void uvmfree(pagetable_t pagetable, uint64 sz);
pagetable_t uvmcopy(pagetable_t old, pagetable_t new, uint64 sz);
void uvmunmap(pagetable_t pagetable, uint64 va, uint64 npages, int do_free);
uint64 uvmalloc(pagetable_t pagetable, uint64 oldsz, uint64 newsz);
uint64 uvmdealloc(pagetable_t pagetable, uint64 oldsz, uint64 newsz);

#endif
