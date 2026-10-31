/* SPDX-FileCopyrightText: 2026 CTRL-F-0rg3
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * CoTanSBI — CLINT (timer) for QEMU 'virt'.
 */

#ifndef COTANOS_SBI_CLINT_H
#define COTANOS_SBI_CLINT_H

#define CLINT_BASE       0x02000000UL  /* CLINT base on 'virt' */
#define CLINT_MTIMECMP   0x4000UL      /* mtimecmp (hart 0) */
#define CLINT_MTIME      0xBFF8UL      /* mtime (shared counter) */

#endif /* COTANOS_SBI_CLINT_H */
