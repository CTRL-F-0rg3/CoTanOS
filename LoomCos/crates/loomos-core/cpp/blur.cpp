#include <cstdint>
#include <cstddef>
#include <memory>
#include <vector>
#include <algorithm>
#include <cstring>

// Internal helper for 1D blur pass
static void blur_pass_1d(
    uint32_t* dest, const uint32_t* src, 
    int32_t len, int32_t stride, int32_t radius
) {
    if (len <= 0 || radius <= 0) {
        if (dest != src) std::memcpy(dest, src, len * sizeof(uint32_t));
        return;
    }

    int32_t r = radius;
    int32_t w = 2 * r + 1;
    
    // Accumulators for ARGB channels
    int32_t sum_a = 0, sum_r = 0, sum_g = 0, sum_b = 0;

    // Initialize window for the first pixel
    for (int i = -r; i <= r; ++i) {
        int32_t idx = std::clamp(i, 0, len - 1);
        uint32_t p = src[idx * stride];
        sum_a += (p >> 24) & 0xFF;
        sum_r += (p >> 16) & 0xFF;
        sum_g += (p >> 8) & 0xFF;
        sum_b += p & 0xFF;
    }

    for (int i = 0; i < len; ++i) {
        dest[i * stride] = ((sum_a / w) << 24) | ((sum_r / w) << 16) | ((sum_g / w) << 8) | (sum_b / w);

        // Slide window: add new right pixel, remove old left pixel
        int32_t add_idx = std::clamp(i + r + 1, 0, len - 1);
        int32_t rem_idx = std::clamp(i - r, 0, len - 1);

        uint32_t p_add = src[add_idx * stride];
        sum_a += ((p_add >> 24) & 0xFF) - ((src[rem_idx * stride] >> 24) & 0xFF);
        sum_r += ((p_add >> 16) & 0xFF) - ((src[rem_idx * stride] >> 16) & 0xFF);
        sum_g += ((p_add >> 8) & 0xFF) - ((src[rem_idx * stride] >> 8) & 0xFF);
        sum_b += (p_add & 0xFF) - (src[rem_idx * stride] & 0xFF);
    }
}

extern "C" void loom_cpu_blur_box_separable(
    uint32_t* dest, int32_t dest_stride,
    const uint32_t* src, int32_t src_stride,
    int32_t width, int32_t height,
    int32_t radius
) {
    if (!dest || !src || width <= 0 || height <= 0) return;

    // Temporary buffer for horizontal pass
    std::vector<uint32_t> temp(width * height);

    // 1. Horizontal pass (src -> temp)
    for (int32_t y = 0; y < height; ++y) {
        blur_pass_1d(
            temp.data() + (y * width), 
            src + (y * src_stride), 
            width, 1, radius
        );
    }

    // 2. Vertical pass (temp -> dest)
    for (int32_t x = 0; x < width; ++x) {
        blur_pass_1d(
            dest + x, 
            temp.data() + x, 
            height, width, radius
        );
    }
}