/* Simple UART output for CoTanOS base library */

#define UART_BASE 0x10000000  /* Standard QEMU UART address */

static inline void uart_putc(char c) {
    volatile char* uart = (volatile char*)UART_BASE;
    *uart = c;
}

void uart_puts(const char* s) {
    while (*s) {
        if (*s == '\n') uart_putc('\r');
        uart_putc(*s++);
    }
}
