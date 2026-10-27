#pragma once
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Error codes for robust error handling across all language bindings
typedef enum {
    LOOM_SUCCESS = 0,
    LOOM_ERR_INVALID_HANDLE = -1,
    LOOM_ERR_OUT_OF_MEMORY = -2,
    LOOM_ERR_BUFFER_MISMATCH = -3,
    LOOM_ERR_TRANSACTION_FAILED = -4
} LoomResult;

// Window styling and decoration flags
typedef enum {
    LOOM_WINDOW_FLAG_DECORATED = 1 << 0,
    LOOM_WINDOW_FLAG_RESIZABLE = 1 << 1,
    LOOM_WINDOW_FLAG_POPUP     = 1 << 2,
    LOOM_WINDOW_FLAG_TRANSPARENT = 1 << 3
} LoomWindowFlags;

// Opaque handles
struct LoomContext;
struct LoomWindow;

typedef struct LoomContext* LoomContextHandle;
typedef struct LoomWindow* LoomWindowHandle;

// --- Context Lifecycle ---
LoomContextHandle loom_context_create(int32_t screen_width, int32_t screen_height);
void loom_context_destroy(LoomContextHandle ctx);

// --- Window Lifecycle & Customization ---
LoomWindowHandle loom_window_create(
    LoomContextHandle ctx, 
    const char* title, 
    int32_t x, 
    int32_t y, 
    int32_t width, 
    int32_t height, 
    uint32_t flags
);

void loom_window_destroy(LoomWindowHandle window);

// --- Glitch-Free Double Buffering & Transaction API ---
// Gets a pointer to the back-buffer pixels for safe rendering without tearing
LoomResult loom_window_lock_buffer(LoomWindowHandle window, uint32_t** out_pixels, int32_t* out_stride);
LoomResult loom_window_unlock_buffer(LoomWindowHandle window);

// ATOMIC COMMIT: Enforces that all buffer updates and geometry changes 
// are synchronized and rendered atomically in the next frame compositor pass.
LoomResult loom_window_commit(LoomWindowHandle window);

// Customization metrics (Window Box styling)
LoomResult loom_window_set_titlebar_height(LoomWindowHandle window, int32_t height);
LoomResult loom_window_set_theme_colors(LoomWindowHandle window, uint32_t bg_color, uint32_t border_color);

#ifdef __cplusplus
}
#endif