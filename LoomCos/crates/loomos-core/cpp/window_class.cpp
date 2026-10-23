#include "loomcos_window.h"
#include <cstring>
#include <algorithm>
#include <string>

// Internal C++ class representing a specialized window type and its decorations
class WindowInstanceInternal {
public:
    LoomcosWindowClassType type;
    int32_t width;
    int32_t height;
    std::string title;
    LoomcosDecorationMetrics metrics;
    bool is_focused;

    WindowInstanceInternal(LoomcosWindowClassType t, int32_t w, int32_t h, const char* t_str)
        : type(t), width(w), height(h), title(t_str ? t_str : ""), is_focused(true) {
        
        // Default modern dark-theme decoration metrics
        metrics.titlebar_height = 28;
        metrics.border_width = 2;
        metrics.button_size = 16;
        metrics.titlebar_color = 0xFF28283D; // Dark slate
        metrics.border_color = 0xFF45475A;   // Accent border
        metrics.text_color = 0xFFCDD6F4;     // Light text
    }

    // Fast software rendering of the titlebar and window frame
    void render_frame(uint32_t* dest, int32_t dest_w, int32_t dest_h, int32_t win_x, int32_t win_y) const {
        if (type != WINDOW_CLASS_STANDARD_DECORATED) {
            return; // Borderless or panels don't render standard window boxes
        }

        // 1. Draw top titlebar box
        int32_t tb_height = metrics.titlebar_height;
        int32_t total_w = width + (metrics.border_width * 2);
        int32_t total_h = height + tb_height + (metrics.border_width * 2);

        // Render borders and titlebar background
        for (int y = 0; y < total_h; ++y) {
            int32_t screen_y = win_y + y;
            if (screen_y < 0 || screen_y >= dest_h) continue;

            for (int x = 0; x < total_w; ++x) {
                int32_t screen_x = win_x + x;
                if (screen_x < 0 || screen_x >= dest_w) continue;

                int32_t dest_idx = screen_y * dest_w + screen_x;

                // Check if pixel belongs to outer border
                bool is_border = (x < metrics.border_width || x >= total_w - metrics.border_width ||
                                  y < metrics.border_width || y >= total_h - metrics.border_width);

                // Check if pixel belongs to titlebar area
                bool is_titlebar = (y >= metrics.border_width && y < metrics.border_width + tb_height &&
                                    !is_border);

                if (is_border) {
                    dest[dest_idx] = metrics.border_color;
                } else if (is_titlebar) {
                    // Titlebar gradient or solid color
                    dest[dest_idx] = is_focused ? metrics.titlebar_color : 0xFF181825;
                }
            }
        }
    }
};

// --- C-ABI FFI Exports ---

extern "C" {

CppWindowInstance* loomcos_window_create(LoomcosWindowClassType type, int32_t width, int32_t height, const char* title) {
    return reinterpret_cast<CppWindowInstance*>(new WindowInstanceInternal(type, width, height, title));
}

void loomcos_window_destroy(CppWindowInstance* win) {
    if (win) {
        delete reinterpret_cast<WindowInstanceInternal*>(win);
    }
}

void loomcos_window_set_metrics(CppWindowInstance* win, const LoomcosDecorationMetrics* metrics) {
    if (win && metrics) {
        auto instance = reinterpret_cast<WindowInstanceInternal*>(win);
        instance->metrics = *metrics;
    }
}

void loomcos_window_render_decorations(
    const CppWindowInstance* win,
    uint32_t* dest,
    int32_t dest_w,
    int32_t dest_h,
    int32_t win_x,
    int32_t win_y
) {
    if (win) {
        const auto instance = reinterpret_cast<const WindowInstanceInternal*>(win);
        instance->render_frame(dest, dest_w, dest_h, win_x, win_y);
    }
}

}