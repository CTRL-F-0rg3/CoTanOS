#pragma once
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Easing / Interpolation curves for natural-looking motion
typedef enum {
    LOOM_EASE_LINEAR = 0,
    LOOM_EASE_IN     = 1,
    LOOM_EASE_OUT    = 2,
    LOOM_EASE_IN_OUT = 3
} LoomEaseType;

// Animation property type
typedef enum {
    LOOM_ANIM_PROPERTY_POSITION = 0, // Interpolates X and Y
    LOOM_ANIM_PROPERTY_SIZE     = 1, // Interpolates Width and Height
    LOOM_ANIM_PROPERTY_OPACITY  = 2  // Interpolates Alpha channel (0 - 255)
} LoomAnimProperty;

// Rectangle definition for animation bounds
typedef struct {
    int32_t x;
    int32_t y;
    int32_t width;
    int32_t height;
} LoomAnimRect;

// Opaque handle for an active animation instance
struct LoomAnimationInstance;
typedef struct LoomAnimationInstance* LoomAnimationHandle;

// --- Animation Lifecycle & Management ---
LoomAnimationHandle loom_anim_create_rect(
    LoomAnimRect start_val, 
    LoomAnimRect target_val, 
    uint32_t duration_frames, 
    LoomEaseType ease
);

LoomAnimationHandle loom_anim_create_float(
    float start_val, 
    float target_val, 
    uint32_t duration_frames, 
    LoomEaseType ease
);

void loom_anim_destroy(LoomAnimationHandle anim);

// Advances animation by one frame and returns current interpolated rectangle values
bool loom_anim_step_rect(LoomAnimationHandle anim, LoomAnimRect* out_current_val);

// Advances animation by one frame and returns current interpolated float/alpha value
bool loom_anim_step_float(LoomAnimationHandle anim, float* out_current_val);

// Checks if animation has completed its duration
bool loom_anim_is_finished(LoomAnimationHandle anim);

#ifdef __cplusplus
}
#endif