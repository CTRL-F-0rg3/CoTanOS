#include "loomcos_alpha.h"
#include <algorithm>

extern "C" {

// Fast per-pixel ARGB alpha blending
void loomcos_cpu_blend_surface(
    uint32_t* dest,
    int32_t dest_w,
    int32_t dest_h,
    const uint32_t* src,
    int32_t src_w,
    int32_t src_h,
    int32_t pos_x,
    int32_t pos_y,
    uint8_t global_alpha
) {
    if (!dest || !src || dest_w <= 0 || dest_h <= 0 || src_w <= 0 || src_h <= 0) return;

    int32_t start_x = std::max(0, pos_x);
    int32_t start_y = std::max(0, pos_y);
    int32_t end_x = std::min(dest_w, pos_x + src_w);
    int32_t end_y = std::min(dest_h, pos_y + src_h);

    if (start_x >= end_x || start_y >= end_y) return;

    for (int y = start_y; y < end_y; ++y) {
        int32_t src_y = y - pos_y;
        int32_t src_x_offset = start_x - pos_x;

        uint32_t* dest_row = dest + (y * dest_w + start_x);
        const uint32_t* src_row = src + (src_y * src_w + src_x_offset);

        for (int x = 0; x < (end_x - start_x); ++x) {
            uint32_t s_pixel = src_row[x];
            
            // Extract source channels (Assuming ARGB: A: 24-31, R: 16-23, G: 8-15, B: 0-7)
            uint8_t s_a = ((s_pixel >> 24) & 0xFF);
            
            // Apply global surface alpha scaling factor
            if (global_alpha < 255) {
                s_a = (s_a * global_alpha) / 255;
            }

            if (s_a == 0) continue; // Fully transparent pixel, skip

            if (s_a == 255) {
                dest_row[x] = s_pixel; // Fully opaque, direct overwrite
            } else {
                // Standard alpha blending (Over operator)
                uint32_t d_pixel = dest_row[x];

                uint8_t s_r = (s_pixel >> 16) & 0xFF;
                uint8_t s_g = (s_pixel >> 8) & 0xFF;
                uint8_t s_b = s_pixel & 0xFF;

                uint8_t d_r = (d_pixel >> 16) & 0xFF;
                uint8_t d_g = (d_pixel >> 8) & 0xFF;
                uint8_t d_b = d_pixel & 0xFF;

                // Precomputed division by 255 approximation or standard math
                uint8_t r = (s_r * s_a + d_r * (255 - s_a)) / 255;
                uint8_t g = (s_g * s_a + d_g * (255 - s_a)) / 255;
                uint8_t b = (s_b * s_a + d_b * (255 - s_a)) / 255;

                // Reconstruct destination pixel (keeping destination alpha or setting to opaque)
                dest_row[x] = (0xFF << 24) | (r << 16) | (g << 8) | b;
            }
        }
    }
}

// Selective alpha blending with color-keying and alpha thresholding
void loomcos_cpu_blend_selective(
    uint32_t* dest,
    int32_t dest_w,
    int32_t dest_h,
    const uint32_t* src,
    int32_t src_w,
    int32_t src_h,
    int32_t pos_x,
    int32_t pos_y,
    uint32_t color_key,
    uint8_t alpha_threshold
) {
    if (!dest || !src || dest_w <= 0 || dest_h <= 0) return;

    int32_t start_x = std::max(0, pos_x);
    int32_t start_y = std::max(0, pos_y);
    int32_t end_x = std::min(dest_w, pos_x + src_w);
    int32_t end_y = std::min(dest_h, pos_y + src_h);

    if (start_x >= end_x || start_y >= end_y) return;

    for (int y = start_y; y < end_y; ++y) {
        int32_t src_y = y - pos_y;
        int32_t src_x_offset = start_x - pos_x;

        uint32_t* dest_row = dest + (y * dest_w + start_x);
        const uint32_t* src_row = src + (src_y * src_w + src_x_offset);

        for (int x = 0; x < (end_x - start_x); ++x) {
            uint32_t s_pixel = src_row[x];

            // Color key check (ignore specific background color completely)
            if ((s_pixel & 0x00FFFFFF) == (color_key & 0x00FFFFFF)) {
                continue;
            }

            uint8_t s_a = (s_pixel >> 24) & 0xFF;

            // Selective alpha thresholding (sharp cutoff for retro / masked assets)
            if (s_a < alpha_threshold) {
                continue;
            }

            if (s_a >= 255) {
                dest_row[x] = s_pixel;
            } else {
                // Fallback to standard blend for partial alpha values above threshold
                uint32_t d_pixel = dest_row[x];
                uint8_t s_r = (s_pixel >> 16) & 0xFF;
                uint8_t s_g = (s_pixel >> 8) & 0xFF;
                uint8_t s_b = s_pixel & 0xFF;

                uint8_t d_r = (d_pixel >> 16) & 0xFF;
                uint8_t d_g = (d_pixel >> 8) & 0xFF;
                uint8_t d_b = d_pixel & 0xFF;

                uint8_t r = (s_r * s_a + d_r * (255 - s_a)) / 255;
                uint8_t g = (s_g * s_a + d_g * (255 - s_a)) / 255;
                uint8_t b = (s_b * s_a + d_b * (255 - s_a)) / 255;

                dest_row[x] = (0xFF << 24) | (r << 16) | (g << 8) | b;
            }
        }
    }
}

}