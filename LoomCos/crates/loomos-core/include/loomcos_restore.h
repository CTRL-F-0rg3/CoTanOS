#pragma once
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Represents an exposed screen region requiring restoration/regeneration
typedef struct {
    int32_t x;
    int32_t y;
    int32_t width;
    int32_t height;
} LoomExposedRegion;

// Clears and repairs an exposed screen region (e.g., after closing a window)
// by repainting it with background color and preparing it for lower layer redrawing.
void loom_cpu_repair_exposed_region(
    uint32_t* dest,
    int32_t dest_w,
    int32_t dest_h,
    const LoomExposedRegion* region,
    uint32_t background_color
);

// Cleans up structural ghost traces left by destroyed window buffers
void loom_cpu_purge_window_ghost(
    uint32_t* dest,
    int32_t dest_w,
    int32_t dest_h,
    int32_t win_x,
    int32_t win_y,
    int32_t win_w,
    int32_t win_h,
    uint32_t fill_color
);

#ifdef __cplusplus
}
#endif

// // Przykład użycia w Rust przy usuwaniu okna z SceneGraph
// pub fn close_window(&mut self, surface_id: SurfaceId) {
//     if let Some(surface) = self.scene.remove(surface_id) {
//         // 1. Purge window ghost from framebuffer immediately using C++ backend
//         unsafe {
//             loom_cpu_purge_window_ghost(
//                 self.framebuffer.as_mut_ptr(),
//                 self.width,
//                 self.height,
//                 surface.bounds.x,
//                 surface.bounds.y,
//                 surface.bounds.width,
//                 surface.bounds.height,
//                 0xFF1E1E2E, // Background color fallback
//             );
//         }

//         // 2. Trigger redraw for windows that were underneath the closed window
//         // (SceneGraph automatically exposes lower layers in the next frame pass)
//     }
// }