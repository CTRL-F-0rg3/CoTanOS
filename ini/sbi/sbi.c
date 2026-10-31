/* SPDX-FileCopyrightText: 2026 CTRL-F-0rg3
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * CoTanSBI — SBI call dispatcher (ECALL from S-mode).
 */

#include "sbi.h"
#include "clint.h"
#include "uart.h"

/* QEMU 'virt': test device — writing 0x5555 powers the machine off. */
#define TEST_DEVICE 0x100000UL

static int sbi_known_extension(unsigned long eid)
{
    switch (eid) {
    case SBI_EXT_BASE:
    case SBI_EXT_PUTCHAR:
    case SBI_EXT_SET_TIMER:
    case SBI_EXT_RESET:
        return 1;
    /* legacy v0.1 */
    case SBI_LEGACY_SET_TIMER:
    case SBI_LEGACY_PUTCHAR:
    case SBI_LEGACY_SHUTDOWN:
        return 1;
    default:
        return 0;
    }
}

static void set_mtimecmp(uint64_t value)
{
    volatile uint64_t *mtimecmp =
        (volatile uint64_t *)(CLINT_BASE + CLINT_MTIMECMP);
    *mtimecmp = value;
    /* Clear a possibly stale STIP so S-mode does not get a duplicated
     * timer interrupt. */
    __asm__ volatile ("csrc mip, %0" : : "r"(1UL << 5));
}

static void system_shutdown(void)
{
    volatile uint32_t *test = (volatile uint32_t *)TEST_DEVICE;
    *test = 0x5555; /* QEMU: exit(0) */
}

uint64_t sbi_handle_ecall(uint64_t eid, uint64_t fid, uint64_t arg)
{
    switch (eid) {
    /* --- SBI v0.2+ --- */
    case SBI_EXT_PUTCHAR:
        if (fid == 2) {
            /* DBCN getchar: return the next character or -1 when empty. */
            return (uint64_t)(long)uart_getc();
        }
        uart_putc((char)arg);
        return SBI_SUCCESS;

    case SBI_EXT_SET_TIMER:
        set_mtimecmp(arg);
        return SBI_SUCCESS;

    case SBI_EXT_RESET:
        if (fid == 0 && (arg == SBI_RESET_SHUTDOWN ||
                         arg == SBI_RESET_COLD_REBOOT)) {
            system_shutdown();
            return SBI_SUCCESS;
        }
        return SBI_ERR_NOT_SUPPORTED;

    case SBI_EXT_BASE:
        switch (fid) {
        case 0:  /* get_spec_version: major << 24 | minor (1.0) */
            return 0x01000000UL;
        case 1:  /* get_sbi_impl_id — own implementation, id 4 (custom) */
            return 4;
        case 3:  /* probe_extension(arg = EID) */
            return sbi_known_extension(arg) ? 1UL : 0UL;
        default:
            return SBI_ERR_NOT_SUPPORTED;
        }

    /* --- SBI legacy v0.1 --- */
    case SBI_LEGACY_SET_TIMER:
        set_mtimecmp(arg);
        return 0;

    case SBI_LEGACY_PUTCHAR:
        uart_putc((char)arg);
        return 0;

    case SBI_LEGACY_SHUTDOWN:
        system_shutdown();
        return 0;

    default:
        return SBI_ERR_NOT_SUPPORTED;
    }
}
