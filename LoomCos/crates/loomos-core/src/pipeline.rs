// src/pipeline.rs (Nowy moduł łączący wszystko)
use crate::ffi;
use crate::surface::{Rect, SurfaceRole};

impl CompositorCore {
    pub fn render_frame(&mut self) {
        // 1. Jeśli nic się nie zmieniło, oszczędzamy CPU (0% użycia w spoczynku!)
        if self.dirty_tracker.is_clean() {
            return; 
        }

        // 2. Pobierz "brudne" obszary ekranu
        let dirty_regions = self.dirty_tracker.take_dirty_regions(self.width, self.height);
        
        // 3. Iteruj po wszystkich oknach (od tyłu do przodu - Z-Order)
        for surface in self.scene.active_surfaces() {
            // A. Occlusion Culling: Sprawdź, co z tego okna jest w ogóle widoczne!
            let visible_parts = self.scene.get_visible_rects_for(surface.id);
            if visible_parts.is_empty() {
                continue; // Okno jest w 100% zasłonięte przez inne, pomijamy je!
            }

            // B. Przecinamy widoczne części okna z "brudnymi" obszarami
            for vis_rect in visible_parts {
                for dirty_rect in &dirty_regions {
                    if let Some(intersection) = intersect_rects(vis_rect, *dirty_rect) {
                        // C. Mamy idealny wycinek do narysowania!
                        // Wywołujemy C++ FFI, aby narysował tylko ten fragment
                        self.blit_surface_clipped(surface, intersection);
                    }
                }
            }
        }

        // 4. Rysowanie dekoracji (Titlebar, Border) - jeśli okno jest "Decorated"
        // 5. Rysowanie kursora (zawsze na wierzchu, w swoim własnym dirty rect)
    }

    fn blit_surface_clipped(&mut self, surface: &Surface, clip: Rect) {
        // Tu wywołujesz C++ FFI, np. loomcos_cpu_blend_surface z odpowiednim clippingiem
    }
}

// Helper do przecinania prostokątów
fn intersect_rects(a: Rect, b: Rect) -> Option<Rect> {
    let x = a.x.max(b.x);
    let y = a.y.max(b.y);
    let w = (a.x + a.width).min(b.x + b.width) - x;
    let h = (a.y + a.height).min(b.y + b.height) - y;
    if w > 0 && h > 0 { Some(Rect::new(x, y, w, h)) } else { None }
}