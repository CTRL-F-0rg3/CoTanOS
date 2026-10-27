#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// High-performance separable box blur (approximates Gaussian).
// Operates in-place or dest/src. Radius is the blur strength.
void loom_cpu_blur_box_separable(
    uint32_t* dest, int32_t dest_stride,
    const uint32_t* src, int32_t src_stride,
    int32_t width, int32_t height,
    int32_t radius
);

#ifdef __cplusplus
}
#endif