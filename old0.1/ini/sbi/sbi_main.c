/* SPDX-FileCopyrightText: 2026 CTRL-F-0rg3
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * CoTanSBI — main M-mode firmware routine.
 * After init it hands off to the bootloader (S-mode) via mret.
 */

#include <stdint.h>
#include "uart.h"

/* CoTanBoot (S-mode bootloader) entry address. */
#define BOOTLOADER_ENTRY 0x80200000UL

/* mstatus: MPP = S-mode (01 -> bit 11 = 0x800), everything else 0. */
#define MSTATUS_MPP_S 0x800UL

/* Original trap cause saved by trap.S (before any handler stack use). */
uint64_t sbi_last_trap[3];

/* Non-static: referenced by name from the naked asm below. */
const char sbi_fatal_msg1[] = "\r\n[CoTanSBI] FATAL M-mode trap! mcause=";
const char sbi_fatal_msg2[] = " mepc=";
const char sbi_fatal_msg3[] = " mtval=";
const char sbi_fatal_msg4[] = "\r\n[CoTanSBI] halting.\r\n";

void sbi_fatal_trap(void) __attribute__((noreturn, naked));
void sbi_fatal_trap(void)
{
    /* Naked: the trapping context may have a garbage sp (e.g. a trap
     * from U-mode) — a normal prologue would fault and re-trap forever. */
    __asm__ volatile(
        "la a0, sbi_fatal_msg1\n"
        "call uart_puts\n"
        "la t0, sbi_last_trap\n"
        "ld a0, 0(t0)\n"
        "call uart_puthex\n"
        "la a0, sbi_fatal_msg2\n"
        "call uart_puts\n"
        "ld a0, 8(t0)\n"
        "call uart_puthex\n"
        "la a0, sbi_fatal_msg3\n"
        "call uart_puts\n"
        "ld a0, 16(t0)\n"
        "call uart_puthex\n"
        "la a0, sbi_fatal_msg4\n"
        "call uart_puts\n"
        "1: wfi\n"
        "j 1b\n");
}

static void __attribute__((noreturn)) enter_supervisor(uint64_t hartid,
                                                       uint64_t fdt)
{
    /*
     * Jump into the bootloader in S-mode:
     *   mepc    = BOOTLOADER_ENTRY
     *   mstatus = MPP=S
     *   a0      = hartid
     *   a1      = FDT address
     */
    __asm__ volatile (
        "csrw mepc, %0\n"
        "csrw mstatus, %1\n"
        "mv a0, %2\n"
        "mv a1, %3\n"
        "mret\n"
        :
        : "r"((uint64_t)BOOTLOADER_ENTRY),
          "r"((uint64_t)MSTATUS_MPP_S),
          "r"(hartid),
          "r"(fdt)
        : "memory");
    __builtin_unreachable();
}

void sbi_main(uint64_t hartid, uint64_t fdt)
{
    uart_init();

    uart_puts("\r\n[CoTanSBI] own M-mode firmware (SBI) — hart ");
    uart_putdec(hartid);
    uart_puts("\r\n[CoTanSBI] traps delegated to S-mode, PMP open, "
              "SBI ECALLs served.\r\n");
    uart_puts("[CoTanSBI] FDT at ");
    uart_puthex(fdt);
    uart_puts("\r\n[CoTanSBI] handing off to CoTanBoot (S-mode) @ ");
    uart_puthex(BOOTLOADER_ENTRY);
    uart_puts("...\r\n");

    enter_supervisor(hartid, fdt);
}
