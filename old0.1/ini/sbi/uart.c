/* SPDX-FileCopyrightText: 2026 CTRL-F-0rg3
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * CoTanSBI — NS16550A UART (polling).
 * On QEMU 'virt' the UART sits at 0x1000_0000 and works out of the box.
 */

#include "uart.h"

#define UART_BASE 0x10000000UL

/* 16550 registers (offset from the base). */
#define UART_RBR 0  /* Receiver Buffer Register (read)   */
#define UART_THR 0  /* Transmit Holding Register (write) */
#define UART_LSR 5  /* Line Status Register */
#define LSR_DR   0x01  /* Data Ready */
#define LSR_THRE 0x20  /* Transmitter Holding Register Empty */

static volatile uint8_t *const UART = (volatile uint8_t *)UART_BASE;

void uart_init(void)
{
    /* On QEMU 'virt' the UART needs no configuration for text output. */
    (void)UART;
}

void uart_putc(char c)
{
    /* Wait until the transmitter is ready (THRE). */
    while (!(UART[UART_LSR] & LSR_THRE)) {
        /* busy-wait */
    }
    UART[UART_THR] = (uint8_t)c;
}

/* Returns the next received character, or -1 when the receiver is empty. */
int uart_getc(void)
{
    if (UART[UART_LSR] & LSR_DR) {
        return (int)UART[UART_RBR];
    }
    return -1;
}

void uart_puts(const char *s)
{
    while (*s) {
        uart_putc(*s++);
    }
}

void uart_puthex(uint64_t v)
{
    static const char digits[] = "0123456789abcdef";
    uart_puts("0x");
    for (int i = 60; i >= 0; i -= 4) {
        uart_putc(digits[(v >> i) & 0xF]);
    }
}

void uart_putdec(uint64_t v)
{
    char buf[24];
    int i = 0;
    if (v == 0) {
        uart_putc('0');
        return;
    }
    while (v > 0) {
        buf[i++] = (char)('0' + (v % 10));
        v /= 10;
    }
    while (i > 0) {
        uart_putc(buf[--i]);
    }
}
