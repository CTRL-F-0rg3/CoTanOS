// crates/loomcos-core/src/lib.rs
pub mod dirty;
pub mod ffi;
pub mod input;
pub mod scenegraph;
pub mod session;
pub mod surface;

use dirty::DirtyTracker;
use input::InputManager;
use scenegraph::SceneGraph;
use session::{ProcessId, ProcessType, WindowSession};
use surface::{Rect, Surface, SurfaceId, SurfaceRole};

pub struct CompositorCore {
    width: i32,
    height: i32,
    framebuffer: Vec<u32>,
    pub scene: SceneGraph,
    sessions: Vec<WindowSession>,
    next_id: u64,
    
    // Added: Core subsystems
    pub dirty_tracker: DirtyTracker,
    pub input_manager: InputManager,
}

impl CompositorCore {
    pub fn new(width: i32, height: i32) -> Self {
        let size = (width * height) as usize;
        Self {
            width,
            height,
            framebuffer: vec![0; size],
            scene: SceneGraph::new(),
            sessions: Vec::new(),
            next_id: 1,
            dirty_tracker: DirtyTracker::new(),
            input_manager: InputManager::new(),
        }
    }

    // ... (istniejący register_window, get_session_by_surface) ...

    // --- Input Event Routing ---

    pub fn process_mouse_down(&mut self, x: i32, y: i32) {
        if let Some(target_id) = self.scene.surface_at_point(x, y) {
            // Bring clicked window to front
            self.scene.bring_to_front(target_id);
            
            // Get surface properties for hit-testing
            if let Some(surface) = self.scene.get_mut(target_id) {
                let bounds = surface.bounds;
                let tb_h = surface.titlebar_height;
                let bw = surface.border_width;
                
                // Let input manager handle the interaction state
                let dirty_rects = self.input_manager.handle_mouse_down(x, y, target_id, bounds, tb_h, bw);
                for rect in dirty_rects {
                    self.dirty_tracker.mark_dirty(rect);
                }
            }
        }
    }

    pub fn process_mouse_move(&mut self, x: i32, y: i32) {
        // Only process move if we are actively dragging or resizing
        let is_interacting = !matches!(self.input_manager.state, input::InteractionState::Idle);
        if !is_interacting {
            return;
        }

        // We need to find the surface currently being manipulated
        let target_id = match &self.input_manager.state {
            input::InteractionState::Dragging { surface_id, .. } => *surface_id,
            input::InteractionState::Resizing { surface_id, .. } => *surface_id,
            _ => return,
        };

        if let Some(surface) = self.scene.get_mut(target_id) {
            let current_bounds = surface.bounds;
            let (new_bounds, dirty_rects) = self.input_manager.handle_mouse_move(x, y, current_bounds);
            
            // Apply new bounds to the surface
            surface.bounds = new_bounds;
            
            // Mark affected regions as dirty
            for rect in dirty_rects {
                self.dirty_tracker.mark_dirty(rect);
            }
        }
    }

    pub fn process_mouse_up(&mut self) {
        self.input_manager.handle_mouse_up();
    }

    // --- Render Pipeline (from previous step) ---

    pub fn render_frame(&mut self) {
        if self.dirty_tracker.is_clean() {
            return;
        }

        let dirty_regions = self.dirty_tracker.take_dirty_regions(self.width, self.height);
        let bg_color = 0xFF1E1E2E; 

        unsafe {
            // 1. Clear dirty regions
            for rect in &dirty_regions {
                ffi::loom_cpu_fill_solid_rect(
                    self.framebuffer.as_mut_ptr(), self.width, self.height,
                    rect.x, rect.y, rect.width, rect.height, bg_color,
                );
            }

            // 2. Render surfaces back-to-front
            for surface in self.scene.active_surfaces() {
                if surface.role == SurfaceRole::Cursor { continue; }

                let visible_rects = self.scene.get_visible_rects_for(surface.id);
                
                for vis_rect in visible_rects {
                    for dirty_rect in &dirty_regions {
                        if let Some(intersection) = Self::intersect_rects(vis_rect, *dirty_rect) {
                            
                            // A. Apply Background Blur if requested (Frosted Glass)
                            if surface.blur_radius > 0 && surface.alpha < 255 {
                                self.apply_blur_to_region(intersection, surface.blur_radius);
                            }

                            // B. Blit the SHM-backed surface
                            self.blit_surface_subrect(surface, intersection);
                        }
                    }
                }
            }

            // 3. Render cursor
            if let Some(cursor) = self.scene.active_surfaces().find(|s| s.role == SurfaceRole::Cursor) {
                for dirty_rect in &dirty_regions {
                    if let Some(intersection) = Self::intersect_rects(cursor.bounds, *dirty_rect) {
                        self.blit_surface_subrect(cursor, intersection);
                    }
                }
            }
        }
    }

    // Extracts background region, blurs it, and writes back to framebuffer
    unsafe fn apply_blur_to_region(&mut self, region: Rect, radius: i32) {
        let w = region.width as usize;
        let h = region.height as usize;
        let mut temp_buf = vec![0u32; w * h];

        // 1. Copy background from framebuffer to temp buffer
        for y in 0..h {
            let src_y = region.y + y as i32;
            let src_offset = (src_y * self.width + region.x) as usize;
            let dst_offset = y * w;
            
            // Safe pointer arithmetic for row copy
            std::ptr::copy_nonoverlapping(
                self.framebuffer.as_ptr().add(src_offset),
                temp_buf.as_mut_ptr().add(dst_offset),
                w,
            );
        }

        // 2. Apply Separable Box Blur (temp_buf -> temp_buf)
        ffi::loom_cpu_blur_box_separable(
            temp_buf.as_mut_ptr(), w as i32,
            temp_buf.as_ptr(), w as i32,
            w as i32, h as i32,
            radius,
        );

        // 3. Write blurred region back to framebuffer
        for y in 0..h {
            let dst_y = region.y + y as i32;
            let dst_offset = (dst_y * self.width + region.x) as usize;
            let src_offset = y * w;
            
            std::ptr::copy_nonoverlapping(
                temp_buf.as_ptr().add(src_offset),
                self.framebuffer.as_mut_ptr().add(dst_offset),
                w,
            );
        }
    }

    // Updated to read from SHM Pool instead of Vec
    unsafe fn blit_surface_subrect(&mut self, surface: &Surface, clip: Rect) {
        let src_x = clip.x - surface.bounds.x;
        let src_y = clip.y - surface.bounds.y;
        let src_offset = (src_y * surface.bounds.width + src_x) as usize;
        
        let total_pixels = (surface.bounds.width * surface.bounds.height) as usize;
        if src_offset + (clip.width * clip.height) as usize > total_pixels {
            return; 
        }
        
        // Read directly from Shared Memory (Zero-Copy)
        let src_ptr = surface.shm_pool.as_ptr().add(src_offset);

        ffi::loomcos_cpu_blend_surface(
            self.framebuffer.as_mut_ptr(), self.width, self.height,
            src_ptr, clip.width, clip.height,
            clip.x, clip.y, surface.alpha,
        );
    }

    fn intersect_rects(a: Rect, b: Rect) -> Option<Rect> {
        let x = a.x.max(b.x);
        let y = a.y.max(b.y);
        let w = (a.x + a.width).min(b.x + b.width) - x;
        let h = (a.y + a.height).min(b.y + b.height) - y;
        if w > 0 && h > 0 { Some(Rect::new(x, y, w, h)) } else { None }
    }
}