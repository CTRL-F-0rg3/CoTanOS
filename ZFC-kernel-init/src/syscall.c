/*
 * syscall.c - trap dispatch + syscall table.
 * Numbers follow Linux riscv64 (asm-generic/unistd.h) so that static
 * musl binaries can eventually run unmodified.
 * CoTanOS GPLv3
 */
#include "CoTan_types.h"
#include "CoTan_uart.h"

/* frame indices, must match trap.S (offset / 8) */
#define TF_A0       10
#define TF_A1       11
#define TF_A2       12
#define TF_A7       17
#define TF_CAUSE    32
#define TF_SEPC     33
#define TF_TVAL     34

#define SSTATUS_SUM (1UL << 18)

#define SYS_write        64
#define SYS_exit         93
#define SYS_exit_group   94
#define SYS_getpid      172
#define SYS_sched_yield 124

#define ENOSYS (-38L)
#define EBADF   (-9L)
#define EFAULT (-14L)

void cotan_trap_dump(u64 *f);

/*
 * Copy from user VA. Requires SUM because we run on the user page table.
 * No fault recovery yet: a bad user pointer kills the kernel. See TODO.
 */
static int copyin(u64 uva, void *dst, u64 len) {
    if (len > 4096) return -1;
    volatile char *src = (volatile char *)uva;
    char *d = (char *)dst;
    __asm__ volatile("csrs sstatus, %0" :: "r"(SSTATUS_SUM) : "memory");
    for (u64 i = 0; i < len; i++) d[i] = src[i];
    __asm__ volatile("csrc sstatus, %0" :: "r"(SSTATUS_SUM) : "memory");
    return 0;
}

static long sys_write(u64 fd, u64 buf, u64 len) {
    if (fd != 1 && fd != 2) return EBADF;
    char tmp[256];
    u64 done = 0;
    while (done < len) {
        u64 chunk = len - done;
        if (chunk > sizeof(tmp)) chunk = sizeof(tmp);
        if (copyin(buf + done, tmp, chunk) < 0) return EFAULT;
        for (u64 i = 0; i < chunk; i++) uart_putc(tmp[i]);
        done += chunk;
    }
    return (long)len;
}

static long do_syscall(u64 n, u64 a0, u64 a1, u64 a2) {
    switch (n) {
    case SYS_write:       return sys_write(a0, a1, a2);
    case SYS_getpid:      return 1;
    case SYS_sched_yield: return 0;
    case 1:               /* legacy: apps/user_test.S "hello" */
        uart_puts("[sys] hello from U-mode\n");
        return 0;
    default:
        uart_puts("[sys] ENOSYS nr="); uart_hex(n); uart_putc('\n');
        return ENOSYS;
    }
}

/* Returns 0 to resume user, nonzero to halt. */
u64 cotan_trap_dispatch(u64 *f) {
    u64 cause = f[TF_CAUSE];

    if (cause >> 63) {
        uart_puts("[trap] interrupt, not handled\n");
        return 1;
    }

    if (cause == 8) {                       /* ecall from U */
        f[TF_SEPC] += 4;                    /* ecall is always 4 bytes */
        u64 n = f[TF_A7];
        if (n == SYS_exit || n == SYS_exit_group) {
            uart_puts("[sys] process exited, code="); uart_hex(f[TF_A0]); uart_putc('\n');
            return 1;                       /* no teardown yet */
        }
        f[TF_A0] = (u64)do_syscall(n, f[TF_A0], f[TF_A1], f[TF_A2]);
        return 0;
    }

    cotan_trap_dump(f);
    uart_puts("[trap] fatal, scause="); uart_hex(cause);
    uart_puts(" sepc="); uart_hex(f[TF_SEPC]);
    uart_puts(" stval="); uart_hex(f[TF_TVAL]); uart_putc('\n');
    return 1;
}
