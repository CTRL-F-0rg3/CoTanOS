#include "loomcos_anim.h"
#include <algorithm>
#include <cmath>

// Internal Animation structure supporting various interpolation targets
struct LoomAnimationInstance {
    bool is_rect_type;
    uint32_t current_frame;
    uint32_t total_frames;
    LoomEaseType ease_type;

    // Rect-based targets (position, bounds)
    LoomAnimRect start_rect;
    LoomAnimRect target_rect;

    // Float-based targets (opacity, scale)
    float start_float;
    float target_float;
};

// Easing function math implementation
static float apply_easing(float t, LoomEaseType type) {
    t = std::clamp(t, 0.0f, 1.0f);
    switch (type) {
        case LOOM_EASE_IN:
            return t * t; // Quadratic acceleration
        case LOOM_EASE_OUT:
            return t * (2.0f - t); // Quadratic deceleration
        case LOOM_EASE_IN_OUT:
            return t < 0.5f ? 2.0f * t * t : -1.0f + (4.0f - 2.0f * t) * t;
        case LOOM_EASE_LINEAR:
        default:
            return t;
    }
}

extern "C" {

LoomAnimationHandle loom_anim_create_rect(
    LoomAnimRect start_val, 
    LoomAnimRect target_val, 
    uint32_t duration_frames, 
    LoomEaseType ease
) {
    auto anim = new LoomAnimationInstance();
    anim->is_rect_type = true;
    anim->current_frame = 0;
    anim->total_frames = std::max(1u, duration_frames);
    anim->ease_type = ease;
    anim->start_rect = start_val;
    anim->target_rect = target_val;
    return reinterpret_cast<LoomAnimationHandle>(anim);
}

LoomAnimationHandle loom_anim_create_float(
    float start_val, 
    float target_val, 
    uint32_t duration_frames, 
    LoomEaseType ease
) {
    auto anim = new LoomAnimationInstance();
    anim->is_rect_type = false;
    anim->current_frame = 0;
    anim->total_frames = std::max(1u, duration_frames);
    anim->ease_type = ease;
    anim->start_float = start_val;
    anim->target_float = target_val;
    return reinterpret_cast<LoomAnimationHandle>(anim);
}

void loom_anim_destroy(LoomAnimationHandle anim) {
    if (anim) {
        delete reinterpret_cast<LoomAnimationInstance*>(anim);
    }
}

bool loom_anim_step_rect(LoomAnimationHandle anim, LoomAnimRect* out_current_val) {
    if (!anim || !out_current_val) return false;
    auto a = reinterpret_cast<LoomAnimationInstance*>(anim);

    if (a->current_frame > a->total_frames) {
        return false; // Animation completed
    }

    float progress = static_cast<float>(a->current_frame) / static_cast<float>(a->total_frames);
    float factor = apply_easing(progress, a->ease_type);

    out_current_val->x = static_cast<int32_t>(a->start_rect.x + factor * (a->target_rect.x - a->start_rect.x));
    out_current_val->y = static_cast<int32_t>(a->start_rect.y + factor * (a->target_rect.y - a->start_rect.y));
    out_current_val->width = static_cast<int32_t>(a->start_rect.width + factor * (a->target_rect.width - a->start_rect.width));
    out_current_val->height = static_cast<int32_t>(a->start_rect.height + factor * (a->target_rect.height - a->start_rect.height));

    a->current_frame++;
    return true;
}

bool loom_anim_step_float(LoomAnimationHandle anim, float* out_current_val) {
    if (!anim || !out_current_val) return false;
    auto a = reinterpret_cast<LoomAnimationInstance*>(anim);

    if (a->current_frame > a->total_frames) {
        return false;
    }

    float progress = static_cast<float>(a->current_frame) / static_cast<float>(a->total_frames);
    float factor = apply_easing(progress, a->ease_type);

    *out_current_val = a->start_float + factor * (a->target_float - a->start_float);

    a->current_frame++;
    return true;
}

bool loom_anim_is_finished(LoomAnimationHandle anim) {
    if (!anim) return true;
    auto a = reinterpret_cast<LoomAnimationInstance*>(anim);
    return a->current_frame > a->total_frames;
}

}