#pragma once
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Performs a differential (selective) blit between a new source buffer and a previous source buffer.
// Automatically skips unmodified scanlines using fast row-level comparison, copying only changed pixels.
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
);

#ifdef __cplusplus
}
#endif