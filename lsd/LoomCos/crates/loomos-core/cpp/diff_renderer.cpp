#include "loomcos_diff.h"
#include <algorithm>
#include <cstring>

extern "C" {

void loom_cpu_blit_differential(
    uint32_t* dest,
    int32_t dest_w,
    int32_t dest_h,
    const uint32_t* src_new,
    const uint32_t* src_old,
    int32_t src_w,
    int32_t src_h,
    int32_t pos_x,
    int32_t pos_y
) {
    if (!dest || !src_new || !src_old || dest_w <= 0 || dest_h <= 0 || src_w <= 0 || src_h <= 0) {
        return;
    }

    // Clip target boundaries to screen dimensions
    int32_t start_x = std::max(0, pos_x);
    int32_t start_y = std::max(0, pos_y);
    int32_t end_x = std::min(dest_w, pos_x + src_w);
    int32_t end_y = std::min(dest_h, pos_y + src_h);

    if (start_x >= end_x || start_y >= end_y) return;

    size_t row_bytes = (end_x - start_x) * sizeof(uint32_t);

    for (int32_t y = start_y; y < end_y; ++y) {
        int32_t src_y = y - pos_y;
        int32_t src_x_offset = start_x - pos_x;

        const uint32_t* new_row = src_new + (src_y * src_w + src_x_offset);
        const uint32_t* old_row = src_old + (src_y * src_w + src_x_offset);

        // FAST ROW-LEVEL CHECK: If the entire row is identical to the previous frame, skip it instantly!
        if (std::memcmp(new_row, old_row, row_bytes) == 0) {
            continue; 
        }

        uint32_t* dest_row = dest + (y * dest_w + start_x);

        // If the row changed, update only the specific pixels that differ
        for (int32_t x = 0; x < (end_x - start_x); ++x) {
            uint32_t pixel_new = new_row[x];
            uint32_t pixel_old = old_row[x];

            // Selective pixel update: only touch destination if the pixel actually changed
            if (pixel_new != pixel_old) {
                // Handle basic alpha blending or direct copy based on alpha channel
                uint8_t alpha = (pixel_new >> 24) & 0xFF;

                if (alpha == 255) {
                    dest_row[x] = pixel_new;
                } else if (alpha > 0) {
                    // Simple blend fallback for partial transparency
                    uint32_t bg = dest_row[x];
                    uint8_t s_r = (pixel_new >> 16) & 0xFF;
                    uint8_t s_g = (pixel_new >> 8) & 0xFF;
                    uint8_t s_b = pixel_new & 0xFF;
                    uint8_t d_r = (bg >> 16) & 0xFF;
                    uint8_t d_g = (bg >> 8) & 0xFF;
                    uint8_t d_b = bg & 0xFF;

                    uint8_t r = (s_r * alpha + d_r * (255 - alpha)) / 255;
                    uint8_t g = (s_g * alpha + d_g * (255 - alpha)) / 255;
                    uint8_t b = (s_b * alpha + d_b * (255 - alpha)) / 255;

                    dest_row[x] = (0xFF << 24) | (r << 16) | (g << 8) | b;
                }
            }
        }
    }
}

}