/*
 * EDID parser for CoTanOS
 * Adapted from Linux DRM EDID parsing (GPLv2/GPLv3 compatible)
 */

#include "base/cotan_shim.h"
#include "base/display.h"

static const uint8_t edid_header[8] = {
    0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00
};

static void parse_vendor_id(const uint8_t *data, char vendor[4]) {
    uint16_t vendor_code = (uint16_t)((data[8] << 8) | data[9]);
    vendor[0] = (char)(((vendor_code >> 10) & 0x1F) + 'A' - 1);
    vendor[1] = (char)(((vendor_code >> 5) & 0x1F) + 'A' - 1);
    vendor[2] = (char)((vendor_code & 0x1F) + 'A' - 1);
    vendor[3] = '\0';
}

static int parse_detailed_timing(const uint8_t *data, display_mode_t *mode) {
    uint32_t pixel_clock_khz;
    uint16_t hactive, hblank, vactive, vblank;
    uint16_t hsync_offset, hsync_width, vsync_offset, vsync_width;
    uint16_t htotal, vtotal;
    
    pixel_clock_khz = (uint32_t)(((data[1] << 8) | data[0]) * 10);
    if (pixel_clock_khz == 0) return -1;
    
    hactive = (uint16_t)(((data[4] & 0xF0) << 4) | data[2]);
    hblank = (uint16_t)(((data[4] & 0x0F) << 8) | data[3]);
    vactive = (uint16_t)(((data[7] & 0xF0) << 4) | data[5]);
    vblank = (uint16_t)(((data[7] & 0x0F) << 8) | data[6]);
    hsync_offset = (uint16_t)(((data[11] & 0xC0) << 2) | data[8]);
    hsync_width = (uint16_t)(((data[11] & 0x30) << 4) | data[9]);
    vsync_offset = (uint16_t)(((data[11] & 0x0C) << 2) | (data[10] >> 4));
    vsync_width = (uint16_t)(((data[11] & 0x03) << 4) | (data[10] & 0x0F));
    
    htotal = hactive + hblank;
    vtotal = vactive + vblank;
    
    mode->hactive = hactive;
    mode->vactive = vactive;
    mode->htotal = htotal;
    mode->vtotal = vtotal;
    mode->hsync_start = (uint16_t)(hactive + hsync_offset);
    mode->hsync_end = (uint16_t)(hactive + hsync_offset + hsync_width);
    mode->vsync_start = (uint16_t)(vactive + vsync_offset);
    mode->vsync_end = (uint16_t)(vactive + vsync_offset + vsync_width);
    mode->pixel_clock_khz = pixel_clock_khz;
    
    if (htotal > 0 && vtotal > 0) {
        mode->refresh_rate = (uint16_t)((pixel_clock_khz * 1000) / ((uint32_t)htotal * vtotal));
    } else {
        mode->refresh_rate = 0;
    }
    
    snprintf(mode->name, sizeof(mode->name), "%ux%u@%u",
             hactive, vactive, mode->refresh_rate);
    
    return 0;
}

int edid_parse(const uint8_t *data, size_t len, edid_t *edid) {
    size_t i;
    uint8_t checksum = 0;
    int mode_count = 0;
    
    if (!data || !edid || len < 128) return -1;
    
    memset(edid, 0, sizeof(*edid));
    
    if (memcmp(data, edid_header, 8) != 0) {
        printf("[edid] invalid header\n");
        return -1;
    }
    
    for (i = 0; i < 128; i++) checksum += data[i];
    if (checksum != 0) {
        printf("[edid] checksum mismatch (sum=%u)\n", checksum);
        return -1;
    }
    
    parse_vendor_id(data, (char *)edid->vendor);
    edid->product_code = (uint16_t)((data[11] << 8) | data[10]);
    edid->serial = (uint32_t)((data[15] << 24) | (data[14] << 16) | 
                              (data[13] << 8) | data[12]);
    edid->width_cm = data[21];
    edid->height_cm = data[22];
    
    edid->modes = NULL;
    edid->preferred_mode = NULL;
    edid->mode_count = 0;
    
    for (i = 0; i < 4; i++) {
        size_t offset = 54 + i * 18;
        if (data[offset] != 0 || data[offset + 1] != 0) {
            display_mode_t *new_modes;
            display_mode_t mode;
            
            if (parse_detailed_timing(&data[offset], &mode) == 0) {
                new_modes = (display_mode_t *)realloc(edid->modes, 
                    (size_t)(mode_count + 1) * sizeof(display_mode_t));
                if (!new_modes) {
                    free(edid->modes);
                    return -1;
                }
                edid->modes = new_modes;
                edid->modes[mode_count] = mode;
                if (mode_count == 0) edid->preferred_mode = &edid->modes[0];
                mode_count++;
            }
        }
    }
    
    edid->mode_count = (size_t)mode_count;
    return 0;
}

void edid_free(edid_t *edid) {
    if (edid) {
        free(edid->modes);
        edid->modes = NULL;
        edid->preferred_mode = NULL;
        edid->mode_count = 0;
    }
}

void edid_dump(const edid_t *edid) {
    size_t i;
    if (!edid) return;
    
    printf("=== EDID Information ===\n");
    printf("Vendor:          %s\n", (char*)edid->vendor);
    printf("Product Code:    0x%04x\n", edid->product_code);
    printf("Serial:          0x%08x\n", edid->serial);
    printf("Size:            %ux%u cm\n", edid->width_cm, edid->height_cm);
    printf("Modes found:     %u\n", (unsigned)edid->mode_count);
    
    for (i = 0; i < edid->mode_count; i++) {
        const display_mode_t *m = &edid->modes[i];
        printf("  [%u] %s (clock=%u kHz)\n", (unsigned)i, m->name, m->pixel_clock_khz);
        printf("       H: active=%u total=%u sync=%u-%u\n",
               m->hactive, m->htotal, m->hsync_start, m->hsync_end);
        printf("       V: active=%u total=%u sync=%u-%u\n",
               m->vactive, m->vtotal, m->vsync_start, m->vsync_end);
    }
    
    if (edid->preferred_mode) {
        printf("Preferred mode:  %s\n", edid->preferred_mode->name);
    }
    printf("========================\n");
}

uint8_t pixel_format_bpp(pixel_format_t format) {
    switch (format) {
        case PIXEL_FORMAT_XRGB8888:
        case PIXEL_FORMAT_ARGB8888: return 4;
        case PIXEL_FORMAT_RGB888: return 3;
        case PIXEL_FORMAT_RGB565: return 2;
        default: return 0;
    }
}

void framebuffer_clear(framebuffer_t *fb, uint32_t color) {
    size_t i;
    uint8_t bpp = pixel_format_bpp(fb->format);
    size_t pixels = (size_t)(fb->width * fb->height);
    
    if (!fb->virt_addr) return;
    
    switch (bpp) {
        case 4: {
            uint32_t *buf = (uint32_t *)fb->virt_addr;
            for (i = 0; i < pixels; i++) buf[i] = color;
            break;
        }
        case 2: {
            uint16_t *buf = (uint16_t *)fb->virt_addr;
            for (i = 0; i < pixels; i++) buf[i] = (uint16_t)color;
            break;
        }
        default: break;
    }
}
