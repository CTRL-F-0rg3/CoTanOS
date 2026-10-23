#include "loomcos_runtime.h"
#include <vector>
#include <string>
#include <mutex>
#include <cstring>
#include <algorithm>

// Internal Window representation with double-buffering for glitch-free rendering
class RuntimeWindow {
public:
    std::string title;
    int32_t x, y, width, height;
    uint32_t flags;
    int32_t titlebar_height;
    uint32_t bg_color;
    uint32_t border_color;

    // Double buffering buffers
    std::vector<uint32_t> front_buffer;
    std::vector<uint32_t> back_buffer;
    std::mutex buffer_mutex;
    bool is_locked;
    bool has_pending_commit;

    RuntimeWindow(const char* t, int32_t px, int32_t py, int32_t w, int32_t h, uint32_t f)
        : title(t ? t : ""), x(px), y(py), width(w), height(h), flags(f),
          titlebar_height(28), bg_color(0xFF1E1E2E), border_color(0xFF45475A),
          is_locked(false), has_pending_commit(false) {
        
        size_t size = w * h;
        front_buffer.resize(size, 0xFF000000);
        back_buffer.resize(size, 0xFF000000);
    }

    void commit_buffers() {
        std::lock_guard<std::mutex> lock(buffer_mutex);
        if (has_pending_commit) {
            // Atomic swap between back and front buffers to eliminate tearing/glitches
            front_buffer = back_buffer;
            has_pending_commit = false;
        }
    }
};

// Internal Context holding all active windows
struct LoomContext {
    int32_t screen_width;
    int32_t screen_height;
    std::vector<RuntimeWindow*> windows;
    std::mutex context_mutex;

    LoomContext(int w, int h) : screen_width(w), screen_height(h) {}
};

// --- C-ABI Implementation ---

extern "C" {

LoomContextHandle loom_context_create(int32_t screen_width, int32_t screen_height) {
    return reinterpret_cast<LoomContextHandle>(new LoomContext(screen_width, screen_height));
}

void loom_context_destroy(LoomContextHandle ctx) {
    if (ctx) {
        auto context = reinterpret_cast<LoomContext*>(ctx);
        for (auto win : context->windows) {
            delete win;
        }
        delete context;
    }
}

LoomWindowHandle loom_window_create(
    LoomContextHandle ctx, 
    const char* title, 
    int32_t x, 
    int32_t y, 
    int32_t width, 
    int32_t height, 
    uint32_t flags
) {
    if (!ctx || width <= 0 || height <= 0) return nullptr;

    auto context = reinterpret_cast<LoomContext*>(ctx);
    auto window = new RuntimeWindow(title, x, y, width, height, flags);

    {
        std::lock_guard<std::mutex> lock(context->context_mutex);
        context->windows.push_back(window);
    }

    return reinterpret_cast<LoomWindowHandle>(window);
}

void loom_window_destroy(LoomWindowHandle window) {
    if (window) {
        auto win = reinterpret_cast<RuntimeWindow*>(window);
        delete win;
    }
}

LoomResult loom_window_lock_buffer(LoomWindowHandle window, uint32_t** out_pixels, int32_t* out_stride) {
    if (!window || !out_pixels || !out_stride) return LOOM_ERR_INVALID_HANDLE;

    auto win = reinterpret_cast<RuntimeWindow*>(window);
    win->buffer_mutex.lock();

    if (win->is_locked) {
        win->buffer_mutex.unlock();
        return LOOM_ERR_TRANSACTION_FAILED;
    }

    win->is_locked = true;
    *out_pixels = win->back_buffer.data();
    *out_stride = win->width;
    return LOOM_SUCCESS;
}

LoomResult loom_window_unlock_buffer(LoomWindowHandle window) {
    if (!window) return LOOM_ERR_INVALID_HANDLE;

    auto win = reinterpret_cast<RuntimeWindow*>(window);
    if (!win->is_locked) {
        return LOOM_ERR_TRANSACTION_FAILED;
    }

    win->is_locked = false;
    win->buffer_mutex.unlock();
    return LOOM_SUCCESS;
}

LoomResult loom_window_commit(LoomWindowHandle window) {
    if (!window) return LOOM_ERR_INVALID_HANDLE;

    auto win = reinterpret_cast<RuntimeWindow*>(window);
    
    // Mark buffer as ready for atomic swap
    win->has_pending_commit = true;
    win->commit_buffers();

    return LOOM_SUCCESS;
}

LoomResult loom_window_set_titlebar_height(LoomWindowHandle window, int32_t height) {
    if (!window || height < 0) return LOOM_ERR_INVALID_HANDLE;
    auto win = reinterpret_cast<RuntimeWindow*>(window);
    win->titlebar_height = height;
    return LOOM_SUCCESS;
}

LoomResult loom_window_set_theme_colors(LoomWindowHandle window, uint32_t bg_color, uint32_t border_color) {
    if (!window) return LOOM_ERR_INVALID_HANDLE;
    auto win = reinterpret_cast<RuntimeWindow*>(window);
    win->bg_color = bg_color;
    win->border_color = border_color;
    return LOOM_SUCCESS;
}

}