#include "loomcos_restore.h"
#include <algorithm>
#include <cstring>

extern "C" {

void loom_cpu_repair_exposed_region(
    uint32_t* dest,
    int32_t dest_w,
    int32_t dest_h,
    const LoomExposedRegion* region,
    uint32_t background_color
) {
    if (!dest || !region || dest_w <= 0 || dest_h <= 0 || region->width <= 0 || region->height <= 0) {
        return;
    }

    // Clip region bounds strictly to physical screen limits
    int32_t start_x = std::max(0, region->x);
    int32_t start_y = std::max(0, region->y);
    int32_t end_x = std::min(dest_w, region->x + region->width);
    int32_t end_y = std::min(dest_h, region->y + region->height);

    if (start_x >= end_x || start_y >= end_y) return;

    // Fast row-by-row background restoration for the exposed hole
    for (int32_t y = start_y; y < end_y; ++y) {
        uint32_t* row = dest + (y * dest_w + start_x);
        int32_t width = end_x - start_x;

        for (int32_t x = 0; x < width; ++x) {
            row[x] = background_color;
        }
    }
}

void loom_cpu_purge_window_ghost(
    uint32_t* dest,
    int32_t dest_w,
    int32_t dest_h,
    int32_t win_x,
    int32_t win_y,
    int32_t win_w,
    int32_t win_h,
    uint32_t fill_color
) {
    if (!dest || dest_w <= 0 || dest_h <= 0 || win_w <= 0 || win_h <= 0) return;

    LoomExposedRegion region;
    region.x = win_x;
    region.y = win_y;
    region.width = win_w;
    region.height = win_h;

    // Re-use the high-speed repair pipeline to completely wipe destroyed window remnants
    loom_cpu_repair_exposed_region(dest, dest_w, dest_h, &region, fill_color);
}

}