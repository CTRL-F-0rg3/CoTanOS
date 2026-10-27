#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Blends a source buffer onto a destination buffer with full per-pixel alpha support (ARGB8888)
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
);

// Blends with selective alpha / color-keying (e.g., treats a specific color as fully transparent, 
// or applies a threshold to alpha channels for sharp bitonal transparency)
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
);

#ifdef __cplusplus
}
#endif