/* SPDX-FileCopyrightText: 2026 CTRL-F-0rg3
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * CoTanOS — structure passed to the kernel by the bootloader (a2).
 */

#ifndef COTANOS_BOT_BOOTINFO_H
#define COTANOS_BOT_BOOTINFO_H

#include <stdint.h>

/* "CoTaBoot" */
#define BOOTINFO_MAGIC   0x436F5461426F6F74ULL
#define BOOTINFO_VERSION 2

struct bootinfo {
    uint64_t magic;         /* BOOTINFO_MAGIC */
    uint64_t version;       /* BOOTINFO_VERSION */
    uint64_t mem_base;      /* start of physical RAM (from FDT) */
    uint64_t mem_size;      /* size of physical RAM (from FDT) */
    uint64_t fdt_addr;      /* adres FDT */
    uint64_t kernel_entry;  /* kernel entry address */
    /* Since version 2: graphics framebuffer (ramfb, 640x480x32). */
    uint64_t fb_addr;       /* 0 = no screen */
    uint32_t fb_width;
    uint32_t fb_height;
    uint32_t fb_bpp;
    uint32_t fb_pitch;
};

#endif /* COTANOS_BOT_BOOTINFO_H */
