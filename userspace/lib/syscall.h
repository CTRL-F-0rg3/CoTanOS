/* CoTanOS userspace syscalls. Numbers = Linux riscv64 (asm-generic). */
#ifndef COTAN_SYSCALL_H
#define COTAN_SYSCALL_H

#define SYS_write        64
#define SYS_exit         93
#define SYS_exit_group   94
#define SYS_sched_yield 124
#define SYS_getpid      172

static inline long syscall3(long n, long a, long b, long c) {
    register long r_a0 asm("a0") = a;
    register long r_a1 asm("a1") = b;
    register long r_a2 asm("a2") = c;
    register long r_a7 asm("a7") = n;
    asm volatile("ecall"
                 : "+r"(r_a0)
                 : "r"(r_a1), "r"(r_a2), "r"(r_a7)
                 : "memory");
    return r_a0;
}

static inline long sys_write(int fd, const void *buf, unsigned long len) {
    return syscall3(SYS_write, fd, (long)buf, (long)len);
}
static inline long sys_getpid(void) { return syscall3(SYS_getpid, 0, 0, 0); }
static inline void sys_exit(int code) {
    syscall3(SYS_exit, code, 0, 0);
    for (;;) {}
}
#endif
