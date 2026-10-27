// crates/loomcos-core/src/scenegraph.rs

use crate::surface::{Rect, Surface, SurfaceId};

pub struct SceneGraph {
    surfaces: Vec<Surface>,
}

impl SceneGraph {
    pub fn new() -> Self {
        Self {
            surfaces: Vec::new(),
        }
    }

    pub fn insert(&mut self, surface: Surface) {
        self.surfaces.push(surface);
        self.sort_z_order();
    }

    pub fn remove(&mut self, id: SurfaceId) -> Option<Surface> {
        if let Some(index) = self.surfaces.iter().position(|s| s.id == id) {
            Some(self.surfaces.remove(index))
        } else {
            None
        }
    }

    pub fn get_mut(&mut self, id: SurfaceId) -> Option<&mut Surface> {
        self.surfaces.iter_mut().find(|s| s.id == id)
    }

    pub fn sort_z_order(&mut self) {
        // Lower z_index renders first (background -> foreground)
        self.surfaces.sort_by_key(|s| s.z_index);
    }

    /// Performs hit-testing to locate which window is under the cursor (front-to-back)
    pub fn surface_at_point(&self, x: i32, y: i32) -> Option<SurfaceId> {
        self.surfaces
            .iter()
            .rev()
            .find(|s| s.visible && s.bounds.contains_point(x, y))
            .map(|s| s.id)
    }

    pub fn active_surfaces(&self) -> impl Iterator<Item = &Surface> {
        self.surfaces.iter().filter(|s| s.visible)
    }

    /// Returns all surfaces that are layered strictly ABOVE the given target surface
    pub fn surfaces_above(&self, target_id: SurfaceId) -> Vec<&Surface> {
        let target_z = match self.surfaces.iter().find(|s| s.id == target_id) {
            Some(s) => s.z_index,
            None => return Vec::new(),
        };

        self.surfaces
            .iter()
            .filter(|s| s.visible && s.z_index >= target_z && s.id != target_id)
            .collect()
    }

    /// Calculates if a specific surface is completely or partially occluded 
    /// by other opaque windows stacked above it.
    pub fn get_visible_rects_for(&self, target_id: SurfaceId) -> Vec<Rect> {
        let target_bounds = match self.surfaces.iter().find(|s| s.id == target_id) {
            Some(s) => s.bounds,
            None => return Vec::new(),
        };

        // Start with the full surface bounds as the initial visible area
        let mut visible_regions = vec![target_bounds];

        // Gather all opaque surfaces rendered on top of this target
        let upper_surfaces = self.surfaces_above(target_id);

        for upper in upper_surfaces {
            // If the upper surface has transparency (alpha < 255), it doesn't fully occlude background
            if upper.alpha < 255 {
                continue;
            }

            let mut next_regions = Vec::new();

            for region in visible_regions {
                // Subtract the upper surface bounds from the current visible region
                subtract_rect(region, upper.bounds, &mut next_regions);
            }

            visible_regions = next_regions;
            if visible_regions.is_empty() {
                break; // Fully occluded, no further calculation needed
            }
        }

        visible_regions
    }
}

/// Helper function to subtract an occluding rectangle from a target region,
/// splitting the remaining visible area into sub-rectangles.
fn subtract_rect(target: Rect, occluder: Rect, output: &mut Vec<Rect>) {
    // Check if rectangles overlap at all
    if target.x >= occluder.x + occluder.width ||
       target.x + target.width <= occluder.x ||
       target.y >= occluder.y + occluder.height ||
       target.y + target.height <= occluder.y 
    {
        output.push(target);
        return;
    }

    // Top slice
    if target.y < occluder.y {
        output.push(Rect::new(
            target.x,
            target.y,
            target.width,
            occluder.y - target.y,
        ));
    }

    // Bottom slice
    let occluder_bottom = occluder.y + occluder.height;
    let target_bottom = target.y + target.height;
    if target_bottom > occluder_bottom {
        output.push(Rect::new(
            target.x,
            occluder_bottom,
            target.width,
            target_bottom - occluder_bottom,
        ));
    }

    let overlap_y_start = std::cmp::max(target.y, occluder.y);
    let overlap_y_end = std::cmp::min(target_bottom, occluder_bottom);

    // Left slice
    if target.x < occluder.x {
        output.push(Rect::new(
            target.x,
            overlap_y_start,
            occluder.x - target.x,
            overlap_y_end - overlap_y_start,
        ));
    }

    // Right slice
    let occluder_right = occluder.x + occluder.width;
    let target_right = target.x + target.width;
    if target_right > occluder_right {
        output.push(Rect::new(
            occluder_right,
            overlap_y_start,
            target_right - occluder_right,
            overlap_y_end - overlap_y_start,
        ));
    }
}

// crates/loomcos-core/src/surface.rs

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum SurfaceRole {
    /// Full application window (owns a main data-processing loop and client connection)
    ToplevelWindow,
    /// Window decoration element (titlebar, borders, minimize/close buttons managed by compositor/helper)
    Decoration,
    /// Transient popup, context menu, or dropdown
    Popup,
    /// Tooltip or temporary hover overlay
    Tooltip,
    /// Mouse cursor overlay
    Cursor,
    /// Wallpaper / desktop background layer
    Background,
}

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub struct SurfaceId(pub u64);

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub struct Rect {
    pub x: i32,
    pub y: i32,
    pub width: i32,
    pub height: i32,
}

impl Rect {
    pub fn new(x: i32, y: i32, width: i32, height: i32) -> Self {
        Self { x, y, width, height }
    }

    pub fn contains_point(&self, px: i32, py: i32) -> bool {
        px >= self.x && px < self.x + self.width && py >= self.y && py < self.y + self.height
    }
}

use crate::shm::ShmPool;

// ... (Rect, SurfaceRole, SurfaceId bez zmian) ...

pub struct Surface {
    pub id: SurfaceId,
    pub role: SurfaceRole,
    pub bounds: Rect,
    pub z_index: i32,
    pub visible: bool,
    pub alpha: u8,
    
    // Replaced Vec<u32> with SHM Pool for Zero-Copy IPC
    pub shm_pool: ShmPool, 
    
    pub parent_id: Option<SurfaceId>,
    
    // Visual effects
    pub blur_radius: i32, 
    pub titlebar_height: i32,
    pub border_width: i32,
}

impl Surface {
    pub fn new(id: SurfaceId, role: SurfaceRole, x: i32, y: i32, width: i32, height: i32) -> Self {
        let pixel_count = (width * height) as usize;
        let shm_name = format!("/loomcos_surf_{}", id.0);
        
        // Allocate SHM. Panics on OOM, which is fine for OS core.
        let shm_pool = ShmPool::create(&shm_name, pixel_count)
            .expect("Failed to allocate SHM for surface");

        Self {
            id,
            role,
            bounds: Rect::new(x, y, width, height),
            z_index: 0,
            visible: true,
            alpha: 255,
            shm_pool,
            parent_id: None,
            blur_radius: 0,
            titlebar_height: 28,
            border_width: 4,
        }
    }
}