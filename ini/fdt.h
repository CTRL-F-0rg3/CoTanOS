/* SPDX-FileCopyrightText: 2026 CTRL-F-0rg3
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * CoTanBoot — minimal Flattened Device Tree parser (v17, big-endian).
 * Only needed: the /memory node and its reg property (RAM base + size).
 */

#ifndef COTANOS_BOT_FDT_H
#define COTANOS_BOT_FDT_H

#include <stdint.h>

/* FDT header (v17). */
struct fdt_header {
    uint32_t magic;
    uint32_t totalsize;
    uint32_t off_dt_struct;
    uint32_t off_dt_strings;
    uint32_t off_mem_rsvmap;
    uint32_t version;
    uint32_t last_comp_version;
    uint32_t boot_cpuid_phys;
    uint32_t size_dt_strings;
    uint32_t size_dt_struct;
};

/* Does the given area look like an FDT? */
int fdt_valid(const void *fdt);

/*
 * Read the RAM base and size from the /memory node ("reg" property).
 * Returns 0 on success, -1 on error.
 */
int fdt_memory(const void *fdt, uint64_t *base, uint64_t *size);

/*
 * Locate the FDT: first try the a1 hint (QEMU),
 * then scan RAM upwards every 4 KiB (older QEMU without a1).
 */
const void *fdt_locate(uint64_t hint);

#endif /* COTANOS_BOT_FDT_H */
