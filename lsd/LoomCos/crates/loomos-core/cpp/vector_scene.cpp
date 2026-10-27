#include "loomcos_vector.h"
#include <vector>
#include <cmath>
#include <algorithm>

struct VectorCommand {
    LoomVectorCmdType type;
    union {
        LoomVecRect rect;
        struct {
            float aspect_ratio;
            LoomAspectMode mode;
            uint32_t color;
        } wallpaper;
    } data;
};

struct LoomVectorScene {
    float grid_w;
    float grid_h;
    std::vector<VectorCommand> commands;

    LoomVectorScene(float w, float h) : grid_w(w > 0.0f ? w : 1000.0f), grid_h(h > 0.0f ? h : 1000.0f) {}
};

extern "C" {

LoomVectorSceneHandle loom_vector_scene_create(float grid_width, float grid_height) {
    return reinterpret_cast<LoomVectorSceneHandle>(new LoomVectorScene(grid_width, grid_height));
}

void loom_vector_scene_destroy(LoomVectorSceneHandle scene) {
    if (scene) {
        delete reinterpret_cast<LoomVectorScene*>(scene);
    }
}

void loom_vector_scene_clear(LoomVectorSceneHandle scene) {
    if (scene) {
        auto sc = reinterpret_cast<LoomVectorScene*>(scene);
        sc->commands.clear();
    }
}

void loom_vector_add_rounded_rect(
    LoomVectorSceneHandle scene, 
    float x, float y, float w, float h, 
    float radius, uint32_t fill_color
) {
    if (!scene) return;
    auto sc = reinterpret_cast<LoomVectorScene*>(scene);

    VectorCommand cmd;
    cmd.type = LOOM_VEC_CMD_RECT_ROUNDED;
    cmd.data.rect.x = x;
    cmd.data.rect.y = y;
    cmd.data.rect.width = w;
    cmd.data.rect.height = h;
    cmd.data.rect.corner_radius = radius;
    cmd.data.rect.fill_color = fill_color;
    
    sc->commands.push_back(cmd);
}

void loom_vector_add_wallpaper_layer(
    LoomVectorSceneHandle scene,
    float orig_aspect_ratio,
    LoomAspectMode aspect_mode,
    uint32_t fill_color
) {
    if (!scene) return;
    auto sc = reinterpret_cast<LoomVectorScene*>(scene);

    VectorCommand cmd;
    cmd.type = LOOM_VEC_CMD_WALLPAPER;
    cmd.data.wallpaper.aspect_ratio = orig_aspect_ratio;
    cmd.data.wallpaper.mode = aspect_mode;
    cmd.data.wallpaper.color = fill_color;

    sc->commands.push_back(cmd);
}

void loom_vector_scene_render(
    LoomVectorSceneHandle scene,
    uint32_t* dest_buffer,
    int32_t physical_width,
    int32_t physical_height
) {
    if (!scene || !dest_buffer || physical_width <= 0 || physical_height <= 0) return;

    auto sc = reinterpret_cast<LoomVectorScene*>(scene);

    // Calculate dynamic scaling factors (Virtual Grid -> Physical Target Screen Pixels)
    float scale_x = static_cast<float>(physical_width) / sc->grid_w;
    float scale_y = static_cast<float>(physical_height) / sc->grid_h;

    for (const auto& cmd : sc->commands) {
        if (cmd.type == LOOM_VEC_CMD_WALLPAPER) {
            // --- VECTOR WALLPAPER SCALING WITH ASPECT RATIO RETENTION ---
            float screen_aspect = static_cast<float>(physical_width) / static_cast<float>(physical_height);
            float target_aspect = cmd.data.wallpaper.aspect_ratio;

            float draw_w = static_cast<float>(physical_width);
            float draw_h = static_cast<float>(physical_height);
            float offset_x = 0.0f;
            float offset_y = 0.0f;

            if (cmd.data.wallpaper.mode == LOOM_ASPECT_COVER) {
                if (screen_aspect > target_aspect) {
                    draw_h = draw_w / target_aspect;
                    offset_y = (static_cast<float>(physical_height) - draw_h) / 2.0f;
                } else {
                    draw_w = draw_h * target_aspect;
                    offset_x = (static_cast<float>(physical_width) - draw_w) / 2.0f;
                }
            } else if (cmd.data.wallpaper.mode == LOOM_ASPECT_CONTAIN) {
                if (screen_aspect > target_aspect) {
                    draw_w = draw_h * target_aspect;
                    offset_x = (static_cast<float>(physical_width) - draw_w) / 2.0f;
                } else {
                    draw_h = draw_w / target_aspect;
                    offset_y = (static_cast<float>(physical_height) - draw_h) / 2.0f;
                }
            }

            // Fill wallpaper area on screen buffer directly
            int32_t start_y = std::max(0, static_cast<int32_t>(offset_y));
            int32_t end_y = std::min(physical_height, static_cast<int32_t>(offset_y + draw_h));
            int32_t start_x = std::max(0, static_cast<int32_t>(offset_x));
            int32_t end_x = std::min(physical_width, static_cast<int32_t>(offset_x + draw_w));

            for (int y = start_y; y < end_y; ++y) {
                uint32_t* row = dest_buffer + (y * physical_width);
                for (int x = start_x; x < end_x; ++x) {
                    row[x] = cmd.data.wallpaper.color;
                }
            }
        } 
        else if (cmd.type == LOOM_VEC_CMD_RECT_ROUNDED) {
            // --- VECTOR SHAPE SCALING (Virtual Grid -> Physical Pixels) ---
            const auto& r = cmd.data.rect;

            // Scale bounding box coordinates directly to destination display DPI
            float phys_x = r.x * scale_x;
            float phys_y = r.y * scale_y;
            float phys_w = r.width * scale_x;
            float phys_h = r.height * scale_y;
            float phys_r = r.corner_radius * ((scale_x + scale_y) * 0.5f);

            int32_t min_x = std::max(0, static_cast<int32_t>(phys_x));
            int32_t min_y = std::max(0, static_cast<int32_t>(phys_y));
            int32_t max_x = std::min(physical_width, static_cast<int32_t>(phys_x + phys_w));
            int32_t max_y = std::min(physical_height, static_cast<int32_t>(phys_y + phys_h));

            // Rasterize crisp vector rounded corners on native screen resolution
            for (int32_t py = min_y; py < max_y; ++py) {
                uint32_t* row = dest_buffer + (py * physical_width);
                for (int32_t px = min_x; px < max_x; ++px) {
                    
                    // Rounded corner distance check
                    float dx = 0.0f;
                    float dy = 0.0f;

                    if (px < phys_x + phys_r) dx = (phys_x + phys_r) - px;
                    else if (px > phys_x + phys_w - phys_r) dx = px - (phys_x + phys_w - phys_r);

                    if (py < phys_y + phys_r) dy = (phys_y + phys_r) - py;
                    else if (py > phys_y + phys_h - phys_r) dy = py - (phys_y + phys_h - phys_r);

                    if (dx * dx + dy * dy <= phys_r * phys_r || (dx == 0.0f || dy == 0.0f)) {
                        row[px] = r.fill_color;
                    }
                }
            }
        }
    }
}

}