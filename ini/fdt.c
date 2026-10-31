/* SPDX-FileCopyrightText: 2026 CTRL-F-0rg3
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * CoTanBoot — minimal FDT parser (no libc).
 */

#include "fdt.h"

#define FDT_MAGIC       0xd00dfeedU
#define FDT_BEGIN_NODE  0x1
#define FDT_END_NODE    0x2
#define FDT_PROP        0x3
#define FDT_NOP         0x4
#define FDT_END         0x9

static uint32_t be32(const void *p)
{
    const uint8_t *b = (const uint8_t *)p;
    return ((uint32_t)b[0] << 24) | ((uint32_t)b[1] << 16) |
           ((uint32_t)b[2] << 8) | (uint32_t)b[3];
}

static uint64_t be64(const void *p)
{
    const uint8_t *b = (const uint8_t *)p;
    return ((uint64_t)be32(b) << 32) | (uint64_t)be32(b + 4);
}

static unsigned long str_len(const char *s)
{
    unsigned long n = 0;
    while (s[n]) {
        n++;
    }
    return n;
}

static int str_cmp(const char *a, const char *b)
{
    while (*a && (*a == *b)) {
        a++;
        b++;
    }
    return (unsigned char)*a - (unsigned char)*b;
}

static int str_pref(const char *a, const char *b)
{
    while (*b) {
        if (*a++ != *b++) {
            return 0;
        }
    }
    return 1;
}

int fdt_valid(const void *fdt)
{
    const struct fdt_header *h = fdt;
    return be32(&h->magic) == FDT_MAGIC;
}

/* Read "cells" (big-endian): 1 cell = 32 bits, 2 cells = 64 bits. */
static uint64_t read_cells(const void *p, int cells)
{
    if (cells == 2) {
        return be64(p);
    }
    return be32(p);
}

int fdt_memory(const void *fdt, uint64_t *base, uint64_t *size)
{
    const struct fdt_header *h = fdt;
    const uint8_t *end = (const uint8_t *)fdt + be32(&h->totalsize);
    const uint32_t *p =
        (const uint32_t *)((const uint8_t *)fdt + be32(&h->off_dt_struct));
    const char *strings =
        (const char *)fdt + be32(&h->off_dt_strings);

    if ((const uint8_t *)p >= end || be32(p) != FDT_BEGIN_NODE) {
        return -1;
    }

    /* Root node name — skip. */
    p += 1 + (str_len((const char *)(p + 1)) + 4) / 4;

    uint32_t addr_cells = 1;
    uint32_t size_cells = 1;

    while ((const uint8_t *)p + 8 <= end) {
        uint32_t tok = be32(p);

        if (tok == FDT_END || tok == FDT_END_NODE) {
            return -1;
        }
        if (tok == FDT_NOP) {
            p++;
            continue;
        }
        if (tok == FDT_BEGIN_NODE) {
            const char *name = (const char *)(p + 1);
            int is_mem = (str_cmp(name, "memory") == 0 ||
                          str_pref(name, "memory@"));
            p += 1 + (str_len(name) + 4) / 4;

            if (is_mem) {
                /* Inside /memory, look for the "reg" property. */
                int got = 0;
                while ((const uint8_t *)p + 8 <= end) {
                    uint32_t t2 = be32(p);
                    if (t2 == FDT_END_NODE) {
                        p++;
                        break;
                    }
                    if (t2 == FDT_NOP) {
                        p++;
                        continue;
                    }
                    if (t2 == FDT_PROP) {
                        uint32_t len = be32(p + 1);
                        uint32_t nameoff = be32(p + 2);
                        const char *pname = strings + nameoff;
                        const void *data = (const void *)(p + 3);
                        if (str_cmp(pname, "reg") == 0) {
                            uint64_t addr = read_cells(data, (int)addr_cells);
                            uint64_t sz = read_cells(
                                (const uint8_t *)data + addr_cells * 4,
                                (int)size_cells);
                            *base = addr;
                            *size = sz;
                            got = 1;
                        }
                        p += 3 + (len + 3) / 4;
                    } else {
                        return -1;
                    }
                }
                return got ? 0 : -1;
            }

            /* Other node — skip the whole subtree. */
            int depth = 1;
            while (depth > 0 && (const uint8_t *)p + 4 <= end) {
                uint32_t t3 = be32(p);
                if (t3 == FDT_BEGIN_NODE) {
                    depth++;
                } else if (t3 == FDT_END_NODE) {
                    depth--;
                } else if (t3 == FDT_PROP) {
                    uint32_t len = be32(p + 1);
                    p += 3 + (len + 3) / 4;
                    continue;
                } else if (t3 == FDT_NOP) {
                    p++;
                    continue;
                } else {
                    return -1;
                }
                p++;
            }
            continue;
        }
        if (tok == FDT_PROP) {
            uint32_t len = be32(p + 1);
            uint32_t nameoff = be32(p + 2);
            const char *pname = strings + nameoff;
            const void *data = (const void *)(p + 3);

            /* Address/size cells of the root node. */
            if (str_cmp(pname, "#address-cells") == 0 && len >= 4) {
                addr_cells = be32(data);
            } else if (str_cmp(pname, "#size-cells") == 0 && len >= 4) {
                size_cells = be32(data);
            }
            p += 3 + (len + 3) / 4;
            continue;
        }
        return -1;
    }
    return -1;
}

const void *fdt_locate(uint64_t hint)
{
    /* Primary path: QEMU >= 6.0 passes the FDT address in a1. */
    if (hint != 0 && fdt_valid((const void *)hint)) {
        return (const void *)hint;
    }

    /*
     * Older QEMU: the FDT sits at the end of RAM. Scan upwards every 4 KiB
     * within a safe window (128 MiB RAM on 'virt'), looking for the magic.
     */
    for (uint64_t a = 0x80200000UL; a < 0x88000000UL; a += 0x1000UL) {
        if (fdt_valid((const void *)a)) {
            return (const void *)a;
        }
    }
    return 0;
}
