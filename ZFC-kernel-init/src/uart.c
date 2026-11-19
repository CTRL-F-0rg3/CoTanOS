#include "CoTan_uart.h"

#define UART0_BASE 0x10000000UL
#define UART_THR   0
#define UART_LSR   5
#define LSR_THRE   0x20

static volatile unsigned char *const uart =
    (volatile unsigned char *)UART0_BASE;

void uart_putc(char c) {
    while ((uart[UART_LSR] & LSR_THRE) == 0) { }
    uart[UART_THR] = (unsigned char)c;
    if (c == '\n') uart_putc('\r');
}

void uart_puts(const char *s) {
    for (; *s; s++) uart_putc(*s);
}

void uart_hex(u64 v) {
    static const char d[] = "0123456789abcdef";
    uart_puts("0x");
    for (int i = 60; i >= 0; i -= 4) uart_putc(d[(v >> i) & 0xF]);
}
