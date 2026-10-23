#pragma once
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Color representation structure (RGBA8888)
typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} LoomColorRGB;

// --- Color Decoding & Conversion ---
// Parses a HEX color string (e.g., "#FF5733" or "FF5733" with optional alpha) into a 32-bit ARGB integer
uint32_t loom_color_from_hex(const char* hex_str);

// Creates a 32-bit ARGB color from individual RGB(A) bytes
uint32_t loom_color_from_rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

// Extracts individual RGBA components from a 32-bit ARGB color integer
void loom_color_to_rgba(uint32_t color, LoomColorRGB* out_rgb);

// --- Solid Color Drawing Primitives ---
// Fills a solid colored rectangle onto a target buffer with boundary clipping
void loom_cpu_fill_solid_rect(
    uint32_t* dest,
    int32_t dest_w,
    int32_t dest_h,
    int32_t rect_x,
    int32_t rect_y,
    int32_t rect_w,
    int32_t rect_h,
    uint32_t color
);

#ifdef __cplusplus
}
#endif