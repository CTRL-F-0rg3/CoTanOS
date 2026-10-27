#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// Opaque handle for the stochastic noise-driven tile scheduler
struct LoomNoiseScheduler;
typedef struct LoomNoiseScheduler* LoomNoiseSchedulerHandle;

// Lifecycle management for the noise refresh scheduler
LoomNoiseSchedulerHandle loom_noise_scheduler_create(int32_t screen_w, int32_t screen_h, int32_t tile_size);
void loom_noise_scheduler_destroy(LoomNoiseSchedulerHandle scheduler);

// Scans buffers to update tile activity noise maps using fast differential block checking
void loom_noise_update_activity_map(
    LoomNoiseSchedulerHandle scheduler,
    const uint32_t* current_fb,
    const uint32_t* previous_fb
);

// Performs non-linear stochastic sparse refresh pass:
// Immediately updates active tiles, and samples a noise-distributed subset of dormant tiles per frame.
void loom_noise_sparse_blit_pass(
    LoomNoiseSchedulerHandle scheduler,
    uint32_t* dest_fb,
    const uint32_t* source_fb,
    uint32_t frame_counter
);

#ifdef __cplusplus
}
#endif