#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Defines specialized window class types
typedef enum {
    WINDOW_CLASS_STANDARD_DECORATED = 0,
    WINDOW_CLASS_BORDERLESS_POPUP   = 1,
    WINDOW_CLASS_SYSTEM_PANEL       = 2,
    WINDOW_CLASS_TOOLTIP            = 3
} LoomcosWindowClassType;

// Structural metrics for window decorations (Window Box)
typedef struct {
    int32_t titlebar_height;
    int32_t border_width;
    int32_t button_size;
    uint32_t titlebar_color;
    uint32_t border_color;
    uint32_t text_color;
} LoomcosDecorationMetrics;

// Opaque handle to a C++ Window Class instance
struct CppWindowInstance;

// FFI Lifecycle functions managed by C++
CppWindowInstance* loomcos_window_create(LoomcosWindowClassType type, int32_t width, int32_t height, const char* title);
void loomcos_window_destroy(CppWindowInstance* win);
void loomcos_window_set_metrics(CppWindowInstance* win, const LoomcosDecorationMetrics* metrics);

// Renders window decorations (titlebar, borders, buttons) directly onto a target buffer
void loomcos_window_render_decorations(
    const CppWindowInstance* win,
    uint32_t* dest,
    int32_t dest_w,
    int32_t dest_h,
    int32_t win_x,
    int32_t win_y
);

#ifdef __cplusplus
}
#endif