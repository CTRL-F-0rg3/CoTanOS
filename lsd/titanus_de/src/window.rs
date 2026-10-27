//! Typy okien i wynik layoutu.
//!
//! Zarządzanie oknami (spawn, fokus, obszary robocze, layout) to **logika**
//! i żyje w C++ (`DeLogic`). Tutaj zostają tylko prymitywy: rodzaj okna i
//! wynik układu, z których korzysta renderer. Nie ma tu żadnego stanu.

use crate::geometry::Rect;

pub type WindowId = u32;

/// Rodzaj okna.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum WindowKind {
    /// Układane automatycznie przez kafelkowanie.
    Tiled,
    /// Swobodne, o własnej geometrii, nad kafelkami.
    Floating,
}

/// Wynik layoutu: jedno okno umieszczone na ekranie (liczone przez logikę C++).
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub struct Placed {
    pub id: WindowId,
    pub kind: WindowKind,
    pub rect: Rect,
}
