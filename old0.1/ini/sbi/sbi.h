/* SPDX-FileCopyrightText: 2026 CTRL-F-0rg3
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * CoTanOS — shared SBI ABI definitions (v0.2+ / v1.0 and legacy v0.1).
 * Used by: CoTanSBI (M-mode), CoTanBoot (S-mode) and the kernel.
 */

#ifndef COTANOS_SBI_H
#define COTANOS_SBI_H

#include <stdint.h>

/* Rozszerzenia SBI v0.2+ (EID). */
#define SBI_EXT_BASE       0x10UL
#define SBI_EXT_PUTCHAR    0x4442434EUL  /* "DBCN" — konsola */
#define SBI_EXT_SET_TIMER  0x54494D45UL  /* "TIME" — timer */
#define SBI_EXT_RESET      0x53525354UL  /* "SRST" — reset/power-off */

/* Legacy v0.1 (EID w a7). */
#define SBI_LEGACY_SET_TIMER  0
#define SBI_LEGACY_PUTCHAR    1
#define SBI_LEGACY_SHUTDOWN   8

/* Kody powrotu. */
#define SBI_SUCCESS              0
#define SBI_ERR_NOT_SUPPORTED   ((unsigned long)-2)

/* Argument resetu (SBI_EXT_RESET, fid 0). */
#define SBI_RESET_SHUTDOWN   0
#define SBI_RESET_COLD_REBOOT 1

/* Helper ECALL from S-mode.
 * Per the SBI ABI the call clobbers registers a0-a7 and t0-t6
 * (s0-s11 are preserved) — hence the full clobber list. */
static inline unsigned long sbi_ecall(unsigned long eid,
                                      unsigned long fid,
                                      unsigned long arg0)
{
    register unsigned long a0 asm("a0") = arg0;
    register unsigned long a6 asm("a6") = fid;
    register unsigned long a7 asm("a7") = eid;

    __asm__ volatile ("ecall"
                      : "+r"(a0)
                      : "r"(a6), "r"(a7)
                      : "a1", "a2", "a3", "a4", "a5",
                        "t0", "t1", "t2", "t3", "t4", "t5", "t6",
                        "memory");
    return a0;
}

static inline void sbi_console_putchar(char c)
{
    sbi_ecall(SBI_EXT_PUTCHAR, 0, (unsigned long)(unsigned char)c);
}

static inline void sbi_set_timer(uint64_t stime_value)
{
    sbi_ecall(SBI_EXT_SET_TIMER, 0, stime_value);
}

static inline void sbi_system_reset(unsigned long reset_type)
{
    sbi_ecall(SBI_EXT_RESET, 0, reset_type);
}

#endif /* COTANOS_SBI_H */
