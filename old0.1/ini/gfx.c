/* SPDX-FileCopyrightText: 2026 CTRL-F-0rg3
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * CoTanBoot — ramfb: a framebuffer in guest RAM displayed by QEMU.
 *
 * fw_cfg (MMIO, QEMU riscv 'virt'):
 *   data : 0x1010_0000  (reads the file bytes one after another)
 *   ctl  : 0x1010_0008  (selector, 16-bit big-endian)
 *   dma  : 0x1010_0010  (adres struktury DMA, 64-bit big-endian)
 *
 * DMA (FWCfgDmaAccess): control BE32 (bity 16..29 = selector,
 * bit 4 = WRITE), length BE32, address BE64 (fizyczny adres bufora).
 */

#include "gfx.h"
#include "font_atlas.h"

#define FW_CFG_DATA  0x10100000UL
#define FW_CFG_CTL   0x10100008UL
#define FW_CFG_DMA   0x10100010UL

#define FW_CFG_FILE_DIR   0x19
#define FW_CFG_DMA_SELECT 0x08
#define FW_CFG_DMA_WRITE  0x10

/* DRM_FORMAT_XRGB8888 (little-endian: 0x00RRGGBB). */
#define FOURCC_XRGB8888 0x34325258UL

struct dma_access {
    uint32_t control;
    uint32_t length;
    uint64_t address;
} __attribute__((packed));

struct ramfb_cfg {
    uint64_t addr;
    uint32_t fourcc;
    uint32_t flags;
    uint32_t width;
    uint32_t height;
    uint32_t stride;
} __attribute__((packed));

static int gfx_ok = 0;

static uint16_t bswap16(uint16_t v)
{
    return (uint16_t)((v >> 8) | (v << 8));
}

static uint32_t bswap32(uint32_t v)
{
    return ((v & 0x000000FFUL) << 24) | ((v & 0x0000FF00UL) << 8) |
           ((v & 0x00FF0000UL) >> 8) | ((v & 0xFF000000UL) >> 24);
}

static uint64_t bswap64(uint64_t v)
{
    return ((uint64_t)bswap32((uint32_t)v) << 32) |
           (uint64_t)bswap32((uint32_t)(v >> 32));
}

static uint8_t fw_read_byte(void)
{
    return *(volatile uint8_t *)FW_CFG_DATA;
}

static uint32_t fw_read_be32(void)
{
    uint32_t v = 0;
    for (int i = 0; i < 4; i++) {
        v = (v << 8) | fw_read_byte();
    }
    return v;
}

static void fw_select(uint16_t selector)
{
    /* Rejestr selektora fw_cfg (MMIO) jest 16-bitowy big-endian. */
    *(volatile uint16_t *)FW_CFG_CTL = bswap16(selector);
}

static int my_strcmp(const char *a, const char *b)
{
    while (*a && (*a == *b)) {
        a++;
        b++;
    }
    return (unsigned char)*a - (unsigned char)*b;
}

/* Szuka pliku w katalogu fw_cfg; zwraca selector (0 = brak). */
static uint16_t fw_find_file(const char *wanted)
{
    fw_select(FW_CFG_FILE_DIR);
    uint32_t count = fw_read_be32();
    if (count > 1024) {
        return 0; /* corrupt/wrong directory — do not loop forever */
    }

    for (uint32_t i = 0; i < count; i++) {
        (void)fw_read_be32();          /* rozmiar */
        uint16_t sel = (uint16_t)(fw_read_byte() << 8);
        sel |= fw_read_byte();
        (void)fw_read_byte();          /* reserved lo */
        (void)fw_read_byte();          /* reserved hi */
        char name[56];
        for (int n = 0; n < 56; n++) {
            name[n] = (char)fw_read_byte();
        }
        name[55] = '\0';
        if (my_strcmp(name, wanted) == 0) {
            return sel;
        }
    }
    return 0;
}

/* Write to an fw_cfg file through DMA (synchronous). */
static int fw_dma_write(uint16_t selector, const void *data, uint32_t length)
{
    struct dma_access dma;
    uint64_t phys = (uint64_t)(uintptr_t)&dma;

    dma.control = bswap32(FW_CFG_DMA_WRITE | FW_CFG_DMA_SELECT |
                          ((uint32_t)selector << 16));
    dma.length  = bswap32(length);
    dma.address = bswap64((uint64_t)(uintptr_t)data);

    /*
     * Ordering guarantee: the DMA struct must be visible in RAM
     * before the DMA-register write triggers the transfer (like wmb() in the
     * Linux driver — RISC-V has a weak memory model).
     */
    __asm__ volatile ("fence w,w" ::: "memory");

    /*
     * DMA register (MMIO): first the HIGH 32 address bits (+0, BE),
     * then the LOW bits (+4, BE) — the low write triggers the transfer.
     * (Protocol used by the Linux driver: iowrite32be x2.)
     */
    volatile uint32_t *dreg = (volatile uint32_t *)FW_CFG_DMA;
    dreg[0] = bswap32((uint32_t)(phys >> 32));
    dreg[1] = bswap32((uint32_t)phys);

    if (bswap32(dma.control) & 0x01) {
        return -1; /* error reported by QEMU */
    }
    return 0;
}

int gfx_init(void)
{
    /*
     * fw_cfg can be flaky (occasional ERROR on the first transfer) —
     * hence up to three attempts before giving up.
     */
    for (int attempt = 0; attempt < 3; attempt++) {
        uint16_t sel = fw_find_file("etc/ramfb");
        if (sel == 0) {
            continue; /* no ramfb device */
        }

        struct ramfb_cfg cfg;
        cfg.addr   = bswap64(GFX_FB_BASE);
        cfg.fourcc = bswap32(FOURCC_XRGB8888);
        cfg.flags  = 0;
        cfg.width  = bswap32(GFX_WIDTH);
        cfg.height = bswap32(GFX_HEIGHT);
        cfg.stride = bswap32(GFX_WIDTH * (GFX_BPP / 8));

        if (fw_dma_write(sel, &cfg, (uint32_t)sizeof(cfg)) == 0) {
            gfx_ok = 1;
            gfx_fill(GFX_COL_BG);
            return 0;
        }
    }
    return -1;
}

int gfx_present(void)
{
    return gfx_ok;
}

uint64_t gfx_fb_addr(void)
{
    return GFX_FB_BASE;
}

static void putpixel(int x, int y, uint32_t color)
{
    volatile uint32_t *fb = (volatile uint32_t *)GFX_FB_BASE;
    if (x < 0 || y < 0 || x >= GFX_WIDTH || y >= GFX_HEIGHT) {
        return;
    }
    fb[(uint64_t)y * GFX_WIDTH + (uint64_t)x] = color;
}

void gfx_fill(uint32_t color)
{
    for (int y = 0; y < GFX_HEIGHT; y++) {
        for (int x = 0; x < GFX_WIDTH; x++) {
            putpixel(x, y, color);
        }
    }
}

void gfx_rect(int x, int y, int w, int h, uint32_t color)
{
    for (int dy = 0; dy < h; dy++) {
        for (int dx = 0; dx < w; dx++) {
            putpixel(x + dx, y + dy, color);
        }
    }
}

void gfx_border(int x, int y, int w, int h, uint32_t color)
{
    gfx_rect(x, y, w, 1, color);
    gfx_rect(x, y + h - 1, w, 1, color);
    gfx_rect(x, y, 1, h, color);
    gfx_rect(x + w - 1, y, 1, h, color);
}

/* ---------------------------------------------------------------------------
 * Text from the DejaVu Sans atlas (4 bpp alpha, anti-aliased).
 * Atlas loaded by QEMU (`-device loader`) at ATLAS_ADDR.
 * ------------------------------------------------------------------------- */

#define ATLAS_ADDR 0x81000000UL

static const uint8_t *const ATLAS = (const uint8_t *)ATLAS_ADDR;

static const struct glyph *find_glyph(const struct font *f, unsigned int cp)
{
    if (cp < 32 || cp >= 32 + 216) {
        return 0;
    }
    return &f->glyphs[cp - 32];
}

/* Blends one pixel: fg over dst, coverage alpha 0..=255. */
static void blend_pixel(int x, int y, uint32_t fg, uint32_t alpha)
{
    if (alpha == 0 || x < 0 || y < 0 || x >= GFX_WIDTH || y >= GFX_HEIGHT) {
        return;
    }
    volatile uint32_t *fb = (volatile uint32_t *)GFX_FB_BASE;
    uint32_t bg = fb[(uint64_t)y * GFX_WIDTH + (uint64_t)x];
    uint32_t inv = 255 - alpha;
    uint32_t r = (((bg >> 16) & 0xFF) * inv + ((fg >> 16) & 0xFF) * alpha) / 255;
    uint32_t g = (((bg >> 8) & 0xFF) * inv + ((fg >> 8) & 0xFF) * alpha) / 255;
    uint32_t b = ((bg & 0xFF) * inv + (fg & 0xFF) * alpha) / 255;
    fb[(uint64_t)y * GFX_WIDTH + (uint64_t)x] =
        (bg & 0xFF000000UL) | (r << 16) | (g << 8) | b;
}

/*
 * Mask alpha: 4 bpp, two pixels per byte, high nibble = left pixel
 * (like mask_alpha() in lsd/uspace/src/gfx/text.rs).
 */
static uint32_t glyph_alpha(int row, int col, const struct glyph *g)
{
    uint32_t index = (uint32_t)row * g->width + (uint32_t)col;
    uint8_t byte = ATLAS[g->offset + (index >> 1)];
    uint8_t nib = (index & 1) ? (byte & 0x0F) : (byte >> 4);
    return (uint32_t)nib * 17; /* 15 * 17 = 255 */
}

static int draw_glyph(int pen_x, int baseline, unsigned int cp,
                      uint32_t color, const struct font *f)
{
    const struct glyph *g = find_glyph(f, cp);
    if (!g) {
        return 8;
    }
    if (g->width == 0 || g->height == 0 || g->length == 0) {
        return g->advance; /* e.g. space */
    }
    int gx = pen_x + g->bearing_x;
    int gy = baseline - g->bearing_y;
    for (int row = 0; row < g->height; row++) {
        for (int col = 0; col < g->width; col++) {
            uint32_t a = glyph_alpha(row, col, g);
            if (a) {
                blend_pixel(gx + col, gy + row, color, a);
            }
        }
    }
    return g->advance;
}

int gfx_text_width_aa(const char *s, const struct font *f)
{
    int w = 0;
    while (*s) {
        const struct glyph *g = find_glyph(f, (unsigned char)*s++);
        w += g ? g->advance : 8;
    }
    return w;
}

void gfx_text_aa(int x, int baseline, const char *s, uint32_t color,
                 const struct font *f)
{
    int px = x;
    while (*s) {
        px += draw_glyph(px, baseline, (unsigned char)*s++, color, f);
    }
}
