#include "loomcos_cpu_asm.h"
#include <algorithm>

extern "C" {

void loom_cpu_accelerated_blend(
    uint32_t* dest,
    int32_t dest_w,
    int32_t dest_h,
    const uint32_t* src,
    int32_t src_w,
    int32_t src_h,
    int32_t pos_x,
    int32_t pos_y
) {
    if (!dest || !src || dest_w <= 0 || dest_h <= 0 || src_w <= 0 || src_h <= 0) {
        return;
    }

    int32_t start_x = std::max(0, pos_x);
    int32_t start_y = std::max(0, pos_y);
    int32_t end_x = std::min(dest_w, pos_x + src_w);
    int32_t end_y = std::min(dest_h, pos_y + src_h);

    if (start_x >= end_x || start_y >= end_y) return;

    size_t copy_width = static_cast<size_t>(end_x - start_x);

    // Row-level blitting accelerated via ASM Non-Temporal Streaming
    for (int32_t y = start_y; y < end_y; ++y) {
        int32_t src_y = y - pos_y;
        int32_t src_x_offset = start_x - pos_x;

        uint32_t* dest_row = dest + (y * dest_w + start_x);
        const uint32_t* src_row = src + (src_y * src_w + src_x_offset);

        // High-speed transfer bypassing cache thrashing
        loom_asm_fast_copy_nt(dest_row, src_row, copy_width);
    }
}

}