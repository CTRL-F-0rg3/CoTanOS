#include "CoTan_types.h"
#include "CoTan_uart.h"

/* Trapframe layout - musi pasować do trap.S */
struct trapframe {
    u64 zero;   // 0
    u64 ra;     // 8
    u64 sp;     // 16
    u64 gp;     // 24
    u64 tp;     // 32
    u64 t0;     // 40
    u64 t1;     // 48
    u64 t2;     // 56
    u64 s0;     // 64
    u64 s1;     // 72
    u64 a0;     // 80
    u64 a1;     // 88
    u64 a2;     // 96
    u64 a3;     // 104
    u64 a4;     // 112
    u64 a5;     // 120
    u64 a6;     // 128
    u64 a7;     // 136
    u64 s2;     // 144
    u64 s3;     // 152
    u64 s4;     // 160
    u64 s5;     // 168
    u64 s6;     // 176
    u64 s7;     // 184
    u64 s8;     // 192
    u64 s9;     // 200
    u64 s10;    // 208
    u64 s11;    // 216
    u64 t3;     // 224
    u64 t4;     // 232
    u64 t5;     // 240
    u64 t6;     // 248
    u64 scause; // 256
    u64 sepc;   // 264
    u64 stval;  // 272
    u64 sstatus;// 280
};

/* Syscall numbers (Linux-compatible subset) */
#define SYS_WRITE   1
#define SYS_EXIT    60

/* Syscall handlers */
static u64 sys_write(u64 fd, u64 buf, u64 count) {
    if (fd != 1 && fd != 2) return -1; // tylko stdout/stderr
    
    char *s = (char *)buf;
    for (u64 i = 0; i < count; i++) {
        uart_putc(s[i]);
    }
    return count;
}

static void sys_exit(u64 code) {
    uart_puts("\n[syscall] exit(");
    uart_hex(code);
    uart_puts(")\n");
    for (;;) { } // TODO: proper process termination
}

void c_trap_handler(struct trapframe *tf) {
    u64 cause = tf->scause;
    
    if (cause == 8) { // ecall from U-mode
        u64 syscall_num = tf->a7;
        u64 ret = 0;
        
        switch (syscall_num) {
            case SYS_WRITE:
                ret = sys_write(tf->a0, tf->a1, tf->a2);
                break;
            case SYS_EXIT:
                sys_exit(tf->a0);
                return; // nie wracamy
            default:
                uart_puts("\n[syscall] unknown syscall ");
                uart_hex(syscall_num);
                uart_putc('\n');
                ret = -1;
        }
        
        // Zwracamy wynik w a0
        tf->a0 = ret;
        // Przesuwamy sepc o 4 (ecall ma 4 bajty)
        tf->sepc += 4;
    } else {
        // Inne wyjątki - dump i hang
        uart_puts("\n[TRAP] scause=");
        uart_hex(cause);
        uart_puts(" sepc=");
        uart_hex(tf->sepc);
        uart_puts(" stval=");
        uart_hex(tf->stval);
        uart_putc('\n');
        for (;;) { }
    }
}
