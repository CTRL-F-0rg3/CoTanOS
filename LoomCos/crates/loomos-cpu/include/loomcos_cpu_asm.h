#pragma once
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// Raw ASM-accelerated buffer fill function
void loom_asm_fast_fill(uint32_t* dest, uint32_t color, size_t count);

// Advanced CPU accelerator routines in C++
void loom_cpu_accelerated_blend(
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