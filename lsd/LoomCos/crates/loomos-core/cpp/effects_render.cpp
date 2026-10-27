#include "loomcos_effects.h"
#include <algorithm>
#include <cmath>
#include <vector>
#include <cstring>

// Internal Static Widget representation
struct LoomStaticWidget {
    int32_t width;
    int32_t height;
    std::vector<uint32_t> buffer;
    bool dirty;

    LoomStaticWidget(int w, int h) : width(w), height(h), buffer(w * h, 0), dirty(true) {}
};

// Helper for linear interpolation between two colors
static uint32_t interpolate_color(uint32_t c1, uint32_t c2, float t) {
    t = std::clamp(t, 0.0f, 1.0f);
    
    uint8_t a1 = (c1 >> 24) & 0xFF, r1 = (c1 >> 16) & 0xFF, g1 = (c1 >> 8) & 0xFF, b1 = c1 & 0xFF;
    uint8_t a2 = (c2 >> 24) & 0xFF, r2 = (c2 >> 16) & 0xFF, g2 = (c2 >> 8) & 0xFF, b2 = c2 & 0xFF;

    uint8_t a = static_cast<uint8_t>(a1 + t * (a2 - a1));
    uint8_t r = static_cast<uint8_t>(r1 + t * (r2 - r1));
    uint8_t g = static_cast<uint8_t>(g1 + t * (g2 - g1));
    uint8_t b = static_cast<uint8_t>(b1 + t * (b2 - b1));

    return (a << 24) | (r << 16) | (g << 8) | b;
}

extern "C" {

void loom_render_styled_box_single_pass(
    uint32_t* dest,
    int32_t dest_w,
    int32_t dest_h,
    const LoomBoxStyle* style
) {
    if (!dest || !style || dest_w <= 0 || dest_h <= 0 || style->width <= 0.0f || style->height <= 0.0f) {
        return;
    }

    // Bounding box estimation including potential shadow padding
    float expand = style->has_shadow ? style->shadow_blur + std::max(std::abs(style->shadow_offset_x), std::abs(style->shadow_offset_y)) : 0.0f;
    
    int32_t min_x = std::max(0, static_cast<int32_t>(style->x - expand));
    int32_t min_y = std::max(0, static_cast<int32_t>(style->y - expand));
    int32_t max_x = std::min(dest_w, static_cast<int32_t>(style->x + style->width + expand));
    int32_t max_y = std::min(dest_h, static_cast<int32_t>(style->y + style->height + expand));

    float rx = style->x;
    float ry = style->y;
    float rw = style->width;
    float rh = style->height;
    float radius = std::max(0.0f, style->corner_radius);
    float bw = std::max(0.0f, style->border_width);

    // Single-pass per-pixel evaluation (Zero Overdraw Loop)
    for (int32_t py = min_y; py < max_y; ++py) {
        uint32_t* row = dest + (py * dest_w);
        float fy = static_cast<float>(py);

        for (int32_t px = min_x; px < max_x; ++px) {
            float fx = static_cast<float>(px);

            // 1. Optional Shadow Pass Calculation
            if (style->has_shadow) {
                float sx = fx - (rx + style->shadow_offset_x);
                float sy = fy - (ry + style->shadow_offset_y);
                // Simple soft shadow distance field approximation
                if (sx >= 0.0f && sx <= rw && sy >= 0.0f && sy <= rh) {
                    // Inside shadow bounding box
                    row[px] = style->shadow_color; // Can be enhanced with gaussian falloff
                    continue;
                }
            }

            // 2. Rounded Box Distance Field Checking
            // Determine distance from pixel to box rounded corners
            float dx = 0.0f;
            float dy = 0.0f;

            if (fx < rx + radius) dx = (rx + radius) - fx;
            else if (fx > rx + rw - radius) dx = fx - (rx + rw - radius);

            if (fy < ry + radius) dy = (ry + radius) - fy;
            else if (fy > ry + rh - radius) dy = fy - (ry + rh - radius);

            float dist_sq = dx * dx + dy * dy;

            // Check if pixel is strictly outside the rounded box shape
            if (dx > 0.0f && dy > 0.0f && dist_sq > radius * radius) {
                continue; // Outside shape, skip (transparent / leaves background untouched)
            }

            // Inside the box shape - Evaluate Fill or Border in ONE PASS
            // Check if pixel falls into border region
            bool is_border = false;
            if (bw > 0.0f) {
                bool near_left = (fx >= rx && fx < rx + bw);
                bool near_right = (fx <= rx + rw && fx > rx + rw - bw);
                bool near_top = (fy >= ry && fy < ry + bw);
                bool near_bottom = (fy <= ry + rh && fy > ry + rh - bw);

                if (near_left || near_right || near_top || near_bottom) {
                    is_border = true;
                }
            }

            if (is_border) {
                row[px] = style->border_color;
            } else {
                // Interior fill: Evaluate Gradient or Solid Color in a single step
                if (style->use_gradient) {
                    float t = style->gradient_vertical 
                        ? (fy - ry) / rh 
                        : (fx - rx) / rw;
                    row[px] = interpolate_color(style->gradient_start_color, style->gradient_end_color, t);
                } else {
                    row[px] = style->solid_color;
                }
            }
        }
    }
}

// --- Static Widget Cache Implementation ---

LoomStaticWidgetHandle loom_widget_cache_create(int32_t width, int32_t height) {
    if (width <= 0 || height <= 0) return nullptr;
    return reinterpret_cast<LoomStaticWidgetHandle>(new LoomStaticWidget(width, height));
}

void loom_widget_cache_destroy(LoomStaticWidgetHandle widget) {
    if (widget) {
        delete reinterpret_cast<LoomStaticWidget*>(widget);
    }
}

uint32_t* loom_widget_get_buffer(LoomStaticWidgetHandle widget) {
    if (!widget) return nullptr;
    auto w = reinterpret_cast<LoomStaticWidget*>(widget);
    w->dirty = true; // Mark as modified when application fetches its buffer
    return w->buffer.data();
}

void loom_widget_mark_dirty(LoomStaticWidgetHandle widget, bool dirty) {
    if (widget) {
        reinterpret_cast<LoomStaticWidget*>(widget)->dirty = dirty;
    }
}

bool loom_widget_is_dirty(LoomStaticWidgetHandle widget) {
    if (!widget) return false;
    return reinterpret_cast<LoomStaticWidget*>(widget)->dirty;
}

void loom_widget_render_cached(
    LoomStaticWidgetHandle widget,
    uint32_t* dest,
    int32_t dest_w,
    int32_t dest_h,
    int32_t pos_x,
    int32_t pos_y
) {
    if (!widget || !dest) return;
    auto w = reinterpret_cast<LoomStaticWidget*>(widget);

    // If widget is not dirty, compositor skips re-evaluating or re-blitting heavy assets!
    // Direct fast copy from widget static cache to screen buffer
    int32_t start_x = std::max(0, pos_x);
    int32_t start_y = std::max(0, pos_y);
    int32_t end_x = std::min(dest_w, pos_x + w->width);
    int32_t end_y = std::min(dest_h, pos_y + w->height);

    if (start_x >= end_x || start_y >= end_y) return;

    for (int32_t y = start_y; y < end_y; ++y) {
        int32_t src_y = y - pos_y;
        int32_t src_x_offset = start_x - pos_x;

        uint32_t* dest_row = dest + (y * dest_w + start_x);
        const uint32_t* src_row = w->buffer.data() + (src_y * w->width + src_x_offset);

        std::memcpy(dest_row, src_row, (end_x - start_x) * sizeof(uint32_t));
    }

    // Clear dirty flag after successful presentation pass
    w->dirty = false;
}

}