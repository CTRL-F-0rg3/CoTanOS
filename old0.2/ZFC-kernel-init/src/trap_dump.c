/*
 * trap_dump.c - full register dump on trap, for early bring-up debugging.
 * CoTanOS GPLv3
 */
#include "CoTan_types.h"
#include "CoTan_uart.h"

static const char *reg_names[32] = {
    "zero", "ra  ", "sp  ", "gp  ", "tp  ", "t0  ", "t1  ", "t2  ",
    "s0  ", "s1  ", "a0  ", "a1  ", "a2  ", "a3  ", "a4  ", "a5  ",
    "a6  ", "a7  ", "s2  ", "s3  ", "s4  ", "s5  ", "s6  ", "s7  ",
    "s8  ", "s9  ", "s10 ", "s11 ", "t3  ", "t4  ", "t5  ", "t6  "
};

/* frame layout must match trap.S */
void cotan_trap_dump(u64 *f) {
    uart_puts("\n--- trap frame ---\n");
    uart_puts("scause ="); uart_hex(f[32]);
    uart_puts(" sepc   ="); uart_hex(f[33]); uart_putc('\n');
    uart_puts("stval  ="); uart_hex(f[34]);
    uart_puts(" sstatus="); uart_hex(f[35]); uart_putc('\n');

    if (f[34] < 0x1000 && f[34] != 0) {
        uart_puts("note: stval looks like NULL + offset\n");
    }

    for (int i = 1; i < 32; i++) {
        uart_puts(reg_names[i]); uart_putc('='); uart_hex(f[i]);
        uart_putc((i % 2 == 1) ? ' ' : '\n');
    }
    uart_puts("------------------\n");
}
