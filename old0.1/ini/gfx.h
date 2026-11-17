/* SPDX-FileCopyrightText: 2026 CTRL-F-0rg3
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * CoTanBoot — ramfb (framebuffer in guest RAM, QEMU `-device ramfb`).
 * Configuration through fw_cfg (the "etc/ramfb" file) using the DMA
 * interface:
 *   - fw_cfg MMIO: data 0x1010_0000 (bajty), ctl 0x1010_0008 (16-bit BE),
 *     dma 0x1010_0010 (64-bit BE — adres struktury FWCfgDmaAccess)
 *   - file directory: selector 0x19 (BE32 count + 64 B entries:
 *     size BE32, select BE16, reserved, name[56])
 * No PCI — works on QEMU riscv 'virt' despite the unmapped BARs.
 */

#ifndef COTANOS_BOT_GFX_H
#define COTANOS_BOT_GFX_H

#include <stdint.h>

#define GFX_WIDTH  640
#define GFX_HEIGHT 480
#define GFX_BPP    32

/* Framebuffer in guest RAM (above the kernel, below the FDT at RAM end). */
#define GFX_FB_BASE 0x85000000UL

/* Colors 0x00RRGGBB — the macOS-style boot screen. */
#define GFX_COL_BG      0x000000UL
#define GFX_COL_WHITE   0xFFFFFFUL

/*
 * Find "etc/ramfb" in fw_cfg, submit our framebuffer through DMA
 * (640x480x32, XRGB8888) and prepare the screen. Returns 0 on success,
 * -1 when there is no ramfb.
 */
int gfx_init(void);

/* Drawing on the framebuffer. */
void gfx_fill(uint32_t color);
void gfx_rect(int x, int y, int w, int h, uint32_t color);
void gfx_border(int x, int y, int w, int h, uint32_t color);

/* Text from the DejaVu Sans atlas (anti-aliased, 4 bpp alpha). */
struct font;

void gfx_text_aa(int x, int baseline, const char *s, uint32_t color,
                 const struct font *f);
int gfx_text_width_aa(const char *s, const struct font *f);

uint64_t gfx_fb_addr(void);
int gfx_present(void);

#endif /* COTANOS_BOT_GFX_H */
