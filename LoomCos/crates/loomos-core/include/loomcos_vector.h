#pragma once
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Wallpaper and SVG Asset Aspect-Ratio scaling modes
typedef enum {
    LOOM_ASPECT_STRETCH = 0,     // Stretches vector mesh to fill screen (ignores ratio)
    LOOM_ASPECT_CONTAIN = 1,     // Scales vector mesh to fit entirely inside screen while preserving aspect ratio
    LOOM_ASPECT_COVER   = 2,     // Scales vector mesh to completely cover screen while preserving ratio (clips overflow)
    LOOM_ASPECT_CENTER  = 3      // Keeps original vector scale centered without scaling
} LoomAspectMode;

// 2D Vector Point on the resolution-independent Virtual Grid
typedef struct {
    float x;
    float y;
} LoomVectorPoint;

// Vector Command Types for Virtual Grid Rendering
typedef enum {
    LOOM_VEC_CMD_RECT_ROUNDED = 0,
    LOOM_VEC_CMD_CIRCLE       = 1,
    LOOM_VEC_CMD_PATH_LINE    = 2,
    LOOM_VEC_CMD_WALLPAPER    = 3
} LoomVectorCmdType;

// Rounded Rectangle Vector Primitive
typedef struct {
    float x, y, width, height;
    float corner_radius;
    uint32_t fill_color;
    uint32_t stroke_color;
    float stroke_width;
} LoomVecRect;

// Opaque handles
struct LoomVectorScene;
typedef struct LoomVectorScene* LoomVectorSceneHandle;

// Lifecycle & Scene Construction
LoomVectorSceneHandle loom_vector_scene_create(float grid_width, float grid_height);
void loom_vector_scene_destroy(LoomVectorSceneHandle scene);
void loom_vector_scene_clear(LoomVectorSceneHandle scene);

// Adding resolution-independent vector primitives
void loom_vector_add_rounded_rect(
    LoomVectorSceneHandle scene, 
    float x, float y, float w, float h, 
    float radius, uint32_t fill_color
);

void loom_vector_add_wallpaper_layer(
    LoomVectorSceneHandle scene,
    float orig_aspect_ratio,
    LoomAspectMode aspect_mode,
    uint32_t fill_color
);

// RASTERIZER CORE: Translates Virtual Grid commands directly to physical resolution screen buffer
// Supports any resolution dynamically (Full HD -> 4K -> 8K -> 128K)
void loom_vector_scene_render(
    LoomVectorSceneHandle scene,
    uint32_t* dest_buffer,
    int32_t physical_width,
    int32_t physical_height
);

#ifdef __cplusplus
}
#endif