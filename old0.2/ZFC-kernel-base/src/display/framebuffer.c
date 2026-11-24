/*
 * Framebuffer helper functions for CoTanOS
 */

#include "base/display.h"
#include <string.h>

/* Get bytes per pixel for format */
uint8_t pixel_format_bpp(pixel_format_t format) {
    switch (format) {
        case PIXEL_FORMAT_XRGB8888:
        case PIXEL_FORMAT_ARGB8888:
            return 4;
        case PIXEL_FORMAT_RGB888:
            return 3;
        case PIXEL_FORMAT_RGB565:
            return 2;
        default:
            return 0;
    }
}

/* Clear framebuffer to a color */
void framebuffer_clear(framebuffer_t *fb, uint32_t color) {
    size_t i;
    uint8_t bpp = pixel_format_bpp(fb->format);
    size_t pixels = fb->width * fb->height;
    
    if (!fb->virt_addr) {
        return;
    }
    
    switch (bpp) {
        case 4: {
            uint32_t *buf = (uint32_t *)fb->virt_addr;
            for (i = 0; i < pixels; i++) {
                buf[i] = color;
            }
            break;
        }
        case 2: {
            uint16_t *buf = (uint16_t *)fb->virt_addr;
            for (i = 0; i < pixels; i++) {
                buf[i] = (uint16_t)color;
            }
            break;
        }
        default:
            break;
    }
}
