#ifndef _BASE_DISPLAY_H
#define _BASE_DISPLAY_H

/* Basic type definitions for freestanding environment */
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;
typedef long long int64_t;
typedef unsigned long size_t;

typedef enum {
    PIXEL_FORMAT_XRGB8888 = 0,
    PIXEL_FORMAT_ARGB8888,
    PIXEL_FORMAT_RGB565,
    PIXEL_FORMAT_RGB888,
} pixel_format_t;

typedef struct {
    char name[32];
    uint16_t hactive;
    uint16_t vactive;
    uint16_t htotal;
    uint16_t vtotal;
    uint16_t hsync_start;
    uint16_t hsync_end;
    uint16_t vsync_start;
    uint16_t vsync_end;
    uint32_t pixel_clock_khz;
    uint16_t refresh_rate;
    uint32_t flags;
} display_mode_t;

typedef struct {
    uint32_t width;
    uint32_t height;
    uint32_t stride;
    pixel_format_t format;
    uint64_t phys_addr;
    void *virt_addr;
    size_t size;
} framebuffer_t;

typedef struct {
    uint8_t vendor[4];
    uint16_t product_code;
    uint32_t serial;
    uint8_t width_cm;
    uint8_t height_cm;
    display_mode_t *preferred_mode;
    display_mode_t *modes;
    size_t mode_count;
} edid_t;

int edid_parse(const uint8_t *data, size_t len, edid_t *edid);
void edid_free(edid_t *edid);
void edid_dump(const edid_t *edid);

int edid_parse_cea_extension(const uint8_t *edid_data, size_t total_len);
int drm_add_standard_modes(edid_t *edid);

uint8_t pixel_format_bpp(pixel_format_t format);
void framebuffer_clear(framebuffer_t *fb, uint32_t color);

#endif /* _BASE_DISPLAY_H */
