#include "CoTan_trap.h"
#include "CoTan_uart.h"

static const char *exc_names[] = {
    "misaligned fetch", "fetch access", "illegal instruction", "breakpoint",
    "misaligned load", "load access", "misaligned store", "store access",
    "ecall from U", "ecall from S", "reserved", "ecall from M",
    "fetch page fault", "load page fault", "reserved", "store page fault",
};

void c_trap_handler(u64 cause, u64 epc, u64 tval) {
    uart_puts("\n[TRAP] scause="); uart_hex(cause);
    uart_puts(" sepc="); uart_hex(epc);
    uart_puts(" stval="); uart_hex(tval);
    uart_putc('\n');
    if ((cause >> 63) == 0) {
        u64 code = cause & 0xF;
        uart_puts("[TRAP] exception: ");
        uart_puts(code < 16 ? exc_names[code] : "unknown");
        uart_putc('\n');
    } else {
        uart_puts("[TRAP] interrupt\n");
    }
    for (;;) { }
}
