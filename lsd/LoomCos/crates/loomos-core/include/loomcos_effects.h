#pragma once
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Gradient Stop definition
typedef struct {
    float position; // 0.0 to 1.0
    uint32_t color;
} LoomGradientStop;

// Single-pass Advanced Box Style (Zero Overdraw Structure)
typedef struct {
    float x, y, width, height;
    float corner_radius;
    
    // Border properties
    float border_width;
    uint32_t border_color;

    // Gradient fill properties (if enabled)
    bool use_gradient;
    uint32_t solid_color;
    uint32_t gradient_start_color;
    uint32_t gradient_end_color;
    bool gradient_vertical; // true = top-to-bottom, false = left-to-right

    // Shadow / Filter properties
    bool has_shadow;
    uint32_t shadow_color;
    float shadow_offset_x;
    float shadow_offset_y;
    float shadow_blur;
} LoomBoxStyle;

// Static Widget Cache Structure (Draw Once mechanism)
struct LoomStaticWidget;
typedef struct LoomStaticWidget* LoomStaticWidgetHandle;

// --- Zero-Overdraw Single-Pass Rendering ---
// Renders a fully styled box (rounded corners, gradient fill, border, and shadow) 
// in a SINGLE memory pass, completely avoiding multi-layer overdraw!
void loom_render_styled_box_single_pass(
    uint32_t* dest,
    int32_t dest_w,
    int32_t dest_h,
    const LoomBoxStyle* style
);

// --- Static Widget Cache Lifecycle ---
LoomStaticWidgetHandle loom_widget_cache_create(int32_t width, int32_t height);
void loom_widget_cache_destroy(LoomStaticWidgetHandle widget);
uint32_t* loom_widget_get_buffer(LoomStaticWidgetHandle widget);
void loom_widget_mark_dirty(LoomStaticWidgetHandle widget, bool dirty);
bool loom_widget_is_dirty(LoomStaticWidgetHandle widget);

// Blits the static widget cache to screen only when dirty
void loom_widget_render_cached(
    LoomStaticWidgetHandle widget,
    uint32_t* dest,
    int32_t dest_w,
    int32_t dest_h,
    int32_t pos_x,
    int32_t pos_y
);

#ifdef __cplusplus
}
#endif