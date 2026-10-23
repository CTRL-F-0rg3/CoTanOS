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

    // Performs hit-testing to locate which window is under the cursor
    pub fn surface_at_point(&self, x: i32, y: i32) -> Option<SurfaceId> {
        self.surfaces
            .iter()
            .rev() // Front-to-back checking for input events
            .find(|s| s.visible && s.bounds.contains_point(x, y))
            .map(|s| s.id)
    }

    pub fn active_surfaces(&self) -> impl Iterator<Item = &Surface> {
        self.surfaces.iter().filter(|s| s.visible)
    }
}