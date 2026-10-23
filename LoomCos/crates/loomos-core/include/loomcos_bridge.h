#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int32_t x;
    int32_t y;
} LoomcosPoint;

// Renders a filled polygon (e.g. a vector cursor like a triangle) with color and alpha blending
void loomcos_cpu_fill_polygon(
    uint32_t* dest,
    int32_t dest_w,
    int32_t dest_h,
    const LoomcosPoint* points,
    int32_t point_count,
    uint32_t color
);

// Blits a sprite with an arbitrary polygon clip mask or alpha channel
void loomcos_cpu_blit_sprite_masked(
    uint32_t* dest,
    int32_t dest_w,
    int32_t dest_h,
    const uint32_t* src,
    int32_t src_w,
    int32_t src_h,
    int32_t pos_x,
    int32_t pos_y
);

#ifdef __cplusplus
}
#endif