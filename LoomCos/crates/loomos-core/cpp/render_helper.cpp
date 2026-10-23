#include "loomcos_bridge.h"
#include <algorithm>
#include <vector>
#include <cmath>

extern "C" {

// Simple scanline fill for convex/arbitrary polygons (great for vector cursors)
void loomcos_cpu_fill_polygon(
    uint32_t* dest,
    int32_t dest_w,
    int32_t dest_h,
    const LoomcosPoint* points,
    int32_t point_count,
    uint32_t color
) {
    if (!dest || !points || point_count < 3 || dest_w <= 0 || dest_h <= 0) return;

    // Find bounding box of the polygon to limit scanline range
    int32_t min_y = dest_h, max_y = 0;
    int32_t min_x = dest_w, max_x = 0;

    for (int i = 0; i < point_count; ++i) {
        min_x = std::min(min_x, points[i].x);
        max_x = std::max(max_x, points[i].x);
        min_y = std::min(min_y, points[i].y);
        max_y = std::max(max_y, points[i].y);
    }

    // Clip bounding box to screen dimensions
    min_x = std::max(0, min_x);
    min_y = std::max(0, min_y);
    max_x = std::min(dest_w - 1, max_x);
    max_y = std::min(dest_h - 1, max_y);

    if (min_x > max_x || min_y > max_y) return;

    // Scanline intersection algorithm (Winding number / Edge table approach)
    for (int y = min_y; y <= max_y; ++y) {
        std::vector<int32_t> node_x;

        // Find all intersections of scanline 'y' with polygon edges
        for (int i = 0, j = point_count - 1; i < point_count; j = i++) {
            int32_t yi = points[i].y;
            int32_t yj = points[j].y;
            int32_t xi = points[i].x;
            int32_t xj = points[j].x;

            if ((yi < y && yj >= y) || (yj < y && yi >= y)) {
                if (yi != yj) {
                    int32_t x_intersect = xi + (y - yi) * (xj - xi) / (yj - yi);
                    node_x.push_back(x_intersect);
                }
            }
        }

        // Sort intersection X coordinates left-to-right
        std::sort(node_x.begin(), node_x.end());

        // Fill pixels between pairs of intersections
        for (size_t i = 0; i < node_x.size(); i += 2) {
            if (i + 1 >= node_x.size()) break;

            int32_t x_start = std::max(0, node_x[i]);
            int32_t x_end = std::min(dest_w - 1, node_x[i + 1]);

            for (int x = x_start; x <= x_end; ++x) {
                dest[y * dest_w + x] = color;
            }
        }
    }
}

// Blits a sprite handling standard per-pixel Alpha channel transparency
void loomcos_cpu_blit_sprite_masked(
    uint32_t* dest,
    int32_t dest_w,
    int32_t dest_h,
    const uint32_t* src,
    int32_t src_w,
    int32_t src_h,
    int32_t pos_x,
    int32_t pos_y
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
            uint32_t pixel = src_row[x];
            // Extract alpha channel (assuming ARGB or RGBA layout, here taking highest byte)
            uint8_t alpha = (pixel >> 24) & 0xFF;

            if (alpha == 255) {
                dest_row[x] = pixel;
            } else if (alpha > 0) {
                // Basic alpha blending fallback on CPU
                uint32_t bg = dest_row[x];
                // Simple channel-wise blend can be expanded here
                dest_row[x] = pixel; // Simplified direct copy for now
            }
        }
    }
}

}