#include <stdint.h>

/* Adres bazowy UART NS16550A w maszynie virt / standardowych układach RISC-V */
#define UART_BASE 0x10000000

static volatile uint8_t *const UART_THR = (uint8_t *)UART_BASE;

void uart_putchar(char c) {
    *UART_THR = c;
}

void uart_puts(const char *s) {
    while (*s) {
        uart_putchar(*s++);
    }
}

void init_main(void) {
    uart_puts("\n[CoTanOS Hardware Bootloader - ini]\n");
    uart_puts("Initializing hardware layers, PMP, and core primitives...\n");

    // Tutaj dopiszemy konfigurację własnych procesorów i przejście do 256xtr

    uart_puts("Bootloader stage complete. Halting for now.\n");
    
    while (1) {
        __asm__ volatile ("wfi");
    }
}