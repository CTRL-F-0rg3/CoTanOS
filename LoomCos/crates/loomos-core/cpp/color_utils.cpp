#include "loomcos_color.h"
#include <algorithm>
#include <cstring>

// Helper to parse a single hex character to its integer value
static uint8_t hex_char_to_int(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return 0;
}

extern "C" {

uint32_t loom_color_from_hex(const char* hex_str) {
    if (!hex_str) return 0xFF000000; // Default opaque black on error

    // Skip leading '#' if present
    if (hex_str[0] == '#') {
        hex_str++;
    }

    size_t len = std::strlen(hex_str);
    uint32_t r = 0, g = 0, b = 0, a = 255;

    if (len == 6) {
        // Format: RRGGBB (assumes full opacity alpha 0xFF)
        r = (hex_char_to_int(hex_str[0]) << 4) | hex_char_to_int(hex_str[1]);
        g = (hex_char_to_int(hex_str[2]) << 4) | hex_char_to_int(hex_str[3]);
        b = (hex_char_to_int(hex_str[4]) << 4) | hex_char_to_int(hex_str[5]);
    } else if (len == 8) {
        // Format: AARRGGBB or RRGGBBAA (standard ARGB mapping here)
        a = (hex_char_to_int(hex_str[0]) << 4) | hex_char_to_int(hex_str[1]);
        r = (hex_char_to_int(hex_str[2]) << 4) | hex_char_to_int(hex_str[3]);
        g = (hex_char_to_int(hex_str[4]) << 4) | hex_char_to_int(hex_str[5]);
        b = (hex_char_to_int(hex_str[6]) << 4) | hex_char_to_int(hex_str[7]);
    }

    return (a << 24) | (r << 16) | (g << 8) | b;
}

uint32_t loom_color_from_rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    return (static_cast<uint32_t>(a) << 24) |
           (static_cast<uint32_t>(r) << 16) |
           (static_cast<uint32_t>(g) << 8)  |
           static_cast<uint32_t>(b);
}

void loom_color_to_rgba(uint32_t color, LoomColorRGB* out_rgb) {
    if (!out_rgb) return;
    out_rgb->a = (color >> 24) & 0xFF;
    out_rgb->r = (color >> 16) & 0xFF;
    out_rgb->g = (color >> 8) & 0xFF;
    out_rgb->b = color & 0xFF;
}

void loom_cpu_fill_solid_rect(
    uint32_t* dest,
    int32_t dest_w,
    int32_t dest_h,
    int32_t rect_x,
    int32_t rect_y,
    int32_t rect_w,
    int32_t rect_h,
    uint32_t color
) {
    if (!dest || dest_w <= 0 || dest_h <= 0 || rect_w <= 0 || rect_h <= 0) return;

    // Clip rectangle to target screen bounds
    int32_t start_x = std::max(0, rect_x);
    int32_t start_y = std::max(0, rect_y);
    int32_t end_x = std::min(dest_w, rect_x + rect_w);
    int32_t end_y = std::min(dest_h, rect_y + rect_h);

    if (start_x >= end_x || start_y >= end_y) return;

    // Fast memory row filling
    for (int32_t y = start_y; y < end_y; ++y) {
        uint32_t* row = dest + (y * dest_w + start_x);
        int32_t width = end_x - start_x;

        for (int32_t x = 0; x < width; ++x) {
            row[x] = color;
        }
    }
}

}