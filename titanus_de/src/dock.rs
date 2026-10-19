//! Dok z aplikacjami po **prawej** stronie ekranu.
//!
//! Pionowy pasek o szerokości z motywu (`theme.dock_width`), pełnej wysokości,
//! z kwadratowymi kafelkami aplikacji układanymi od góry. Kafelki są oddzielone
//! stałym odstępem; zaznaczony jest wpis skupiony.

use crate::geometry::{Point, Rect, Size};
use crate::theme::Theme;

/// Jeden wpis doku (aplikacja).
#[derive(Debug, Clone)]
pub struct DockEntry {
    pub id: u32,
    pub name: String,
}

/// Dok po prawej stronie.
#[derive(Debug, Clone, Default)]
pub struct Dock {
    pub entries: Vec<DockEntry>,
    /// Wpis podświetlony jako aktywny, jeśli jest.
    pub focused: Option<u32>,
    next_id: u32,
}

impl Dock {
    pub fn new() -> Self {
        Self {
            entries: Vec::new(),
            focused: None,
            next_id: 1,
        }
    }

    /// Dodaje aplikację i zwraca jej id.
    pub fn add(&mut self, name: impl Into<String>) -> u32 {
        let id = self.next_id;
        self.next_id += 1;
        self.entries.push(DockEntry { id, name: name.into() });
        id
    }

    /// Prostokąt całego doku (prawa krawędź ekranu, pełna wysokość).
    pub fn rect(&self, screen: Size, theme: &Theme) -> Rect {
        Rect::new(
            (screen.w - theme.dock_width) as i32,
            0,
            theme.dock_width,
            screen.h,
        )
    }

    /// Prostokąt kafelki o danym indeksie.
    ///
    /// Kafelka jest kwadratem o boku `dock_width - 16`, zaczyna się 16 px od
    /// góry doku, a kolejne kafelki dzieli 12 px przerwy.
    pub fn entry_rect(&self, index: usize, screen: Size, theme: &Theme) -> Rect {
        let dock = self.rect(screen, theme);
        let side = theme.dock_width.saturating_sub(16);
        let y = dock.y + 16 + (index as i32) * (side as i32 + 12);
        Rect::new(dock.x + 8, y, side, side)
    }

    /// Wpis pod punktem ekranu, jeśli trafia w kafekę.
    pub fn hit(&self, p: Point, screen: Size, theme: &Theme) -> Option<u32> {
        for (i, e) in self.entries.iter().enumerate() {
            if self.entry_rect(i, screen, theme).contains(p) {
                return Some(e.id);
            }
        }
        None
    }
}
