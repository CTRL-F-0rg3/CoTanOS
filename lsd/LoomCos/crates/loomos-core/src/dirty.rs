// crates/loomcos-core/src/dirty.rs

use crate::surface::Rect;

pub struct DirtyTracker {
    dirty_rects: Vec<Rect>,
    is_fully_dirty: bool,
}

impl DirtyTracker {
    pub fn new() -> Self {
        Self {
            dirty_rects: Vec::new(),
            is_fully_dirty: true, // Force full redraw on startup
        }
    }

    /// Mark a specific region as needing a redraw
    pub fn mark_dirty(&mut self, rect: Rect) {
        if self.is_fully_dirty {
            return;
        }
        self.dirty_rects.push(rect);
    }

    /// Force a full screen redraw (e.g., wallpaper change, resolution switch)
    pub fn invalidate_all(&mut self) {
        self.is_fully_dirty = true;
        self.dirty_rects.clear();
    }

    pub fn is_clean(&self) -> bool {
        !self.is_fully_dirty && self.dirty_rects.is_empty()
    }

    pub fn take_dirty_regions(&mut self, screen_width: i32, screen_height: i32) -> Vec<Rect> {
        if self.is_fully_dirty {
            self.is_fully_dirty = false;
            return vec![Rect::new(0, 0, screen_width, screen_height)];
        }

        let regions = std::mem::take(&mut self.dirty_rects);
        // Optional: Here you can merge overlapping rects to reduce blit passes
        regions
    }
}

// pub fn render_frame(&mut self) {
//     // 1. Check if anything actually changed (surfaces, cursor, wallpaper)
//     if self.dirty_tracker.is_clean() {
//         return; // Skip frame completely, zero CPU usage!
//     }

//     // 2. Fetch all regions that need repainting
//     let dirty_regions = self.dirty_tracker.take_dirty_regions(self.width, self.height);
    
//     // Convert Rust rects to FFI C structs for high-speed C++ execution
//     let ffi_rects: Vec<LoomcosRect> = dirty_regions.iter().map(|r| LoomcosRect {
//         x: r.x,
//         y: r.y,
//         width: r.width,
//         height: r.height,
//     }).collect();

//     unsafe {
//         // 3. Clear only the dirty areas with wallpaper/background color
//         loomcos_cpu_clear_rects(
//             self.framebuffer.as_mut_ptr(),
//             self.width,
//             self.height,
//             ffi_rects.as_ptr(),
//             ffi_rects.len() as i32,
//             0xFF1E1E2E, // Default dark theme background color
//         );

//         // 4. Blit visible surface parts that intersect with dirty areas
//         for surface in self.scene.active_surfaces() {
//             loomcos_cpu_blit_dirty(
//                 self.framebuffer.as_mut_ptr(),
//                 self.width,
//                 self.height,
//                 surface.buffer.as_ptr(),
//                 surface.bounds.width,
//                 surface.bounds.height,
//                 surface.bounds.x,
//                 surface.bounds.y,
//                 ffi_rects.as_ptr(),
//                 ffi_rects.len() as i32,
//             );
//         }
//     }
// }