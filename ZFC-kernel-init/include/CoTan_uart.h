#ifndef COTAN_UART_H
#define COTAN_UART_H
#include "CoTan_types.h"

/* QEMU virt: ns16550a @ 0x10000000, reg-shift = 0 */
void uart_putc(char c);
void uart_puts(const char *s);
void uart_hex(u64 v);
#endif
