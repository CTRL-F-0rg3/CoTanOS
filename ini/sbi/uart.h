/* SPDX-FileCopyrightText: 2026 CTRL-F-0rg3
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * CoTanSBI — NS16550A UART (QEMU 'virt': 0x1000_0000).
 */

#ifndef COTANOS_SBI_UART_H
#define COTANOS_SBI_UART_H

#include <stdint.h>

void uart_init(void);
void uart_putc(char c);
void uart_puts(const char *s);
void uart_puthex(uint64_t v);
void uart_putdec(uint64_t v);
int uart_getc(void);

#endif /* COTANOS_SBI_UART_H */
