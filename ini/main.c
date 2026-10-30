#include <stdint.h>

/* ================================================================== */
/*  UART (debug)                                                      */
/* ================================================================== */
#define UART_BASE 0x10000000
static volatile uint8_t *const UART_THR = (uint8_t *)UART_BASE;

void uart_puts(const char *s) {
    while (*s) {
        *UART_THR = *s++;
    }
}

/* ================================================================== */
/*  Prosta czcionka 8x8                                               */
/* ================================================================== */
static const uint8_t font8x8_basic[128][8] = {
    ['C'] = { 0x3C, 0x66, 0xC0, 0xC0, 0xC0, 0x66, 0x3C, 0x00 },
    ['o'] = { 0x00, 0x00, 0x3C, 0x66, 0x66, 0x66, 0x3C, 0x00 },
    ['T'] = { 0x7E, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00 },
    ['a'] = { 0x00, 0x00, 0x3C, 0x06, 0x3E, 0x66, 0x3F, 0x00 },
    ['n'] = { 0x00, 0x00, 0xDC, 0x66, 0x66, 0x66, 0x66, 0x00 },
    ['O'] = { 0x3C, 0x66, 0xC3, 0xC3, 0xC3, 0x66, 0x3C, 0x00 },
    ['S'] = { 0x3C, 0x60, 0x3C, 0x06, 0x06, 0x66, 0x3C, 0x00 },
};

/* ================================================================== */
/*  Framebuffer                                                       */
/* ================================================================== */
typedef struct {
    uint32_t *base;
    uint32_t width;
    uint32_t height;
} Framebuffer;

static inline void put_pixel(Framebuffer *fb, uint32_t x, uint32_t y, uint32_t color) {
    if (x < fb->width && y < fb->height) {
        fb->base[y * fb->width + x] = color;
    }
}

void draw_char(Framebuffer *fb, uint32_t x, uint32_t y, char c, uint32_t color, int scale) {
    const uint8_t *glyph = font8x8_basic[(unsigned char)c];
    for (int row = 0; row < 8; row++) {
        uint8_t line = glyph[row];
        for (int col = 0; col < 8; col++) {
            if (line & (1 << (7 - col))) {
                for (int sx = 0; sx < scale; sx++) {
                    for (int sy = 0; sy < scale; sy++) {
                        put_pixel(fb, x + col * scale + sx, y + row * scale + sy, color);
                    }
                }
            }
        }
    }
}

void draw_centered_string(Framebuffer *fb, const char *str, uint32_t color, int scale) {
    for (uint32_t y = 0; y < fb->height; y++) {
        for (uint32_t x = 0; x < fb->width; x++) {
            fb->base[y * fb->width + x] = 0x00000000;
        }
    }

    int len = 0;
    const char *s = str;
    while (*s++) len++;

    uint32_t char_width   = 8 * scale;
    uint32_t spacing      = 4 * scale;
    uint32_t total_width  = len * char_width + (len > 1 ? (len - 1) * spacing : 0);
    uint32_t total_height = 8 * scale;

    uint32_t start_x = (fb->width  > total_width)  ? (fb->width  - total_width)  / 2 : 0;
    uint32_t start_y = (fb->height > total_height) ? (fb->height - total_height) / 2 : 0;

    uint32_t cursor_x = start_x;
    s = str;
    while (*s) {
        draw_char(fb, cursor_x, start_y, *s, color, scale);
        cursor_x += char_width + spacing;
        s++;
    }
}

/* ================================================================== */
/*  bswap BEZ __builtin_bswap*, BEZ libgcc                            */
/*  __asm__ volatile po obliczeniu blokuje pattern-matcher GCC,       */
/*  który od -O1 zamieniłby to z powrotem na wywołanie __bswap* z libgcc. */
/* ================================================================== */
static __attribute__((noinline)) uint16_t bswap16(uint16_t v) {
    uint16_t r = (uint16_t)((v >> 8) | (v << 8));
    __asm__ volatile ("" : "+r"(r));
    return r;
}

static __attribute__((noinline)) uint32_t bswap32(uint32_t v) {
    uint32_t r = ((v >> 24) & 0x000000FFu) |
                 ((v >>  8) & 0x0000FF00u) |
                 ((v <<  8) & 0x00FF0000u) |
                 ((v << 24) & 0xFF000000u);
    __asm__ volatile ("" : "+r"(r));
    return r;
}

static __attribute__((noinline)) uint64_t bswap64(uint64_t v) {
    uint32_t lo = (uint32_t)(v & 0xFFFFFFFFu);
    uint32_t hi = (uint32_t)(v >> 32);
    uint64_t r  = ((uint64_t)bswap32(lo) << 32) | (uint64_t)bswap32(hi);
    __asm__ volatile ("" : "+r"(r));
    return r;
}

/* ================================================================== */
/*  fw_cfg DMA + ramfb                                                */
/* ================================================================== */
#define FW_CFG_BASE          0x10100000ULL
#define FW_CFG_DMA           (FW_CFG_BASE + 0x10)

#define FW_CFG_ID            0x0001
#define FW_CFG_FILE_DIR      0x0019

#define FW_CFG_DMA_ERROR     0x01
#define FW_CFG_DMA_READ      0x02
#define FW_CFG_DMA_SKIP      0x04
#define FW_CFG_DMA_SELECT    0x08
#define FW_CFG_DMA_WRITE     0x10

struct __attribute__((packed)) FWCfgDmaAccess {
    uint32_t control;
    uint32_t length;
    uint64_t address;
};

struct __attribute__((packed)) RAMFBCfg {
    uint64_t addr;
    uint32_t fourcc;
    uint32_t flags;
    uint32_t width;
    uint32_t height;
    uint32_t stride;
};

static volatile uint64_t *const fw_dma = (volatile uint64_t *)FW_CFG_DMA;
static struct FWCfgDmaAccess dma_req;

static void fw_cfg_dma_transfer(uint32_t control, uint32_t len, uint64_t addr) {
    dma_req.control = bswap32(control);
    dma_req.length  = bswap32(len);
    dma_req.address = bswap64(addr);

    *fw_dma = bswap64((uint64_t)(uintptr_t)&dma_req);

    while (1) {
        uint32_t ctrl = bswap32(dma_req.control);
        if (ctrl & FW_CFG_DMA_ERROR) {
            uart_puts("[fw_cfg] DMA error!\n");
            return;
        }
        if (ctrl == 0) {
            return;
        }
    }
}

static int fw_cfg_has_dma(void) {
    uint32_t id_raw = 0;
    fw_cfg_dma_transfer((FW_CFG_ID << 16) | FW_CFG_DMA_SELECT | FW_CFG_DMA_READ,
                        sizeof(id_raw), (uint64_t)(uintptr_t)&id_raw);

    const uint8_t *p = (const uint8_t *)&id_raw;
    uint32_t val = (uint32_t)p[0]
                 | ((uint32_t)p[1] << 8)
                 | ((uint32_t)p[2] << 16)
                 | ((uint32_t)p[3] << 24);
    return (val & 0x02u) ? 1 : 0;
}

struct __attribute__((packed)) FWCfgFile {
    uint32_t size;
    uint16_t select;
    uint16_t reserved;
    char     name[56];
};

static uint32_t fw_cfg_find_file(const char *name) {
    /* 1. Odczytaj liczbę wpisów (4B, big-endian). SELECT+READ czyta od offsetu 0. */
    uint32_t count_be = 0;
    fw_cfg_dma_transfer((FW_CFG_FILE_DIR << 16) | FW_CFG_DMA_SELECT | FW_CFG_DMA_READ,
                        sizeof(count_be), (uint64_t)(uintptr_t)&count_be);
    uint32_t count = bswap32(count_be);

    /* sanity check – katalog fw_cfg ma zawsze kilka wpisów, ale nie tysiące */
    if (count == 0 || count > 256) {
        return 0xFFFFFFFFu;
    }

    /* 2. Iteruj po wpisach. KLUCZ: nie używamy SELECT przy READ — inaczej     *
     *    fw_cfg_select() wyzerowałby cur_offset z powrotem do 0 i zawsze      *
     *    czytalibyśmy wpis 0. SKIP w pierwszej transakcji ustawia offset,     *
     *    a READ bez SELECT kontynuuje od tego miejsca.                        */
    for (uint32_t i = 0; i < count; i++) {
        struct FWCfgFile file;
        uint32_t offset = sizeof(uint32_t) + i * sizeof(struct FWCfgFile);

        fw_cfg_dma_transfer((FW_CFG_FILE_DIR << 16) | FW_CFG_DMA_SELECT | FW_CFG_DMA_SKIP,
                            offset, 0);

        fw_cfg_dma_transfer(FW_CFG_DMA_READ, sizeof(file),
                            (uint64_t)(uintptr_t)&file);

        int match = 1;
        for (int j = 0; j < 56; j++) {
            if (name[j] == '\0' && file.name[j] == '\0') break;
            if (name[j] != file.name[j]) { match = 0; break; }
        }
        if (match) {
            return bswap16(file.select);
        }
    }
    return 0xFFFFFFFFu;
}

static int ramfb_setup(uint64_t fb_addr, uint32_t width, uint32_t height) {
    if (!fw_cfg_has_dma()) {
        uart_puts("[ramfb] fw_cfg DMA not available\n");
        return 0;
    }

    uint32_t select = fw_cfg_find_file("etc/ramfb");
    if (select == 0xFFFFFFFFu) {
        uart_puts("[ramfb] etc/ramfb not found\n");
        return 0;
    }

    struct RAMFBCfg cfg;
    cfg.addr   = bswap64(fb_addr);
    cfg.fourcc = bswap32(0x34325258u);  /* 'XR24' = DRM_FORMAT_XRGB8888 */
    cfg.flags  = bswap32(0);
    cfg.width  = bswap32(width);
    cfg.height = bswap32(height);
    cfg.stride = bswap32(width * 4);

    fw_cfg_dma_transfer((select << 16) | FW_CFG_DMA_SELECT | FW_CFG_DMA_WRITE,
                        sizeof(cfg), (uint64_t)(uintptr_t)&cfg);

    uart_puts("[ramfb] configured successfully\n");
    return 1;
}

/* ================================================================== */
/*  Główna procedura                                                  */
/* ================================================================== */
void init_main(void) {
    uart_puts("\n[CoTanOS ini] Start inicjalizacji...\n");

    Framebuffer fb;
    fb.base   = (uint32_t *)0x80400000;
    fb.width  = 800;
    fb.height = 600;

    draw_centered_string(&fb, "CoTanOS", 0xFFFFFFFF, 4);
    uart_puts("[CoTanOS ini] Napis wyrenderowany w buforze.\n");

    if (!ramfb_setup((uint64_t)(uintptr_t)fb.base, fb.width, fb.height)) {
        uart_puts("[CoTanOS ini] Nie udalo sie skonfigurowac ramfb.\n");
    }

    while (1) {
        __asm__ volatile ("wfi");
    }
}