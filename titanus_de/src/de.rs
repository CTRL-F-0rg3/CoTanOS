//! `Titanus` — środowisko spięte w jednej strukturze.
//!
//! Podział ról: **rdzeń** (geometria, motyw, tapety, render, dok, pasek) jest
//! w Rust, a **logika DE** (okna, obszary robocze, fokus, layout) żyje w C++
//! ([`DeLogic`]). `Titanus` trzyma `DeLogic` i deleguje do niego każdą
//! operację zarządzania oknami, więc stan logiki nie jest duplikowany w Rust.

use crate::dock::Dock;
use crate::geometry::{Point, Rect, Size};
use crate::logic::DeLogic;
use crate::service::ServiceManager;
use crate::theme::Theme;
use crate::tray::Tray;
use crate::wallpaper::{self, DecodedImage, Fit};
use crate::window::{Placed, WindowId};

/// Środowisko graficzne Titanus.
pub struct Titanus {
    pub screen: Size,
    pub theme: Theme,
    /// Logika DE (C++).
    pub logic: DeLogic,
    /// Przerwa między kafelkami (konfiguracja layoutu).
    pub gap: u32,
    pub dock: Dock,
    pub tray: Tray,
    /// Rejestr usług (daemonów) i autostartu.
    pub services: ServiceManager,
    /// Zdekodowana tapeta (albo `None` — gradient zastępczy).
    pub wallpaper: Option<DecodedImage>,
    pub wallpaper_fit: Fit,
    pub cursor: Point,
}

impl Titanus {
    pub fn new(screen: Size) -> Self {
        let mut dock = Dock::new();
        for name in ["Terminal", "Pliki", "Edytor", "Ustawienia", "Usługi"] {
            dock.add(name);
        }

        Self {
            screen,
            theme: Theme::dark(),
            logic: DeLogic::new(),
            gap: 8,
            dock,
            tray: Tray::default(),
            services: ServiceManager::default(),
            wallpaper: None,
            wallpaper_fit: Fit::Cover,
            cursor: Point::ZERO,
        }
    }

    pub fn spawn_tiled(&mut self, title: impl Into<String>) -> WindowId {
        self.logic.spawn_tiled(&title.into())
    }

    pub fn spawn_floating(&mut self, title: impl Into<String>, rect: Rect) -> WindowId {
        self.logic.spawn_floating(&title.into(), rect)
    }

    pub fn close_window(&mut self, id: WindowId) {
        self.logic.close_window(id);
    }

    pub fn focus(&mut self, id: WindowId) {
        self.logic.focus(id);
    }

    pub fn focus_next(&mut self) {
        self.logic.focus_next();
    }

    pub fn focus_prev(&mut self) {
        self.logic.focus_prev();
    }

    pub fn focused(&self) -> Option<WindowId> {
        self.logic.focused()
    }

    pub fn switch_workspace(&mut self, index: usize) {
        self.logic.switch_workspace(index);
    }

    pub fn move_window_to_workspace(&mut self, id: WindowId, index: usize) {
        self.logic.move_window_to_workspace(id, index);
    }

    pub fn workspace_count(&self) -> usize {
        self.logic.workspace_count()
    }

    pub fn window_title(&self, id: WindowId) -> String {
        self.logic.window_title(id)
    }

    /// Ustawia tapetę z bufora PNG i jej dopasowanie.
    pub fn set_wallpaper(&mut self, bytes: &[u8], fit: Fit) -> Result<(), String> {
        let img = wallpaper::decode_png(bytes)?;
        self.wallpaper = Some(img);
        self.wallpaper_fit = fit;
        Ok(())
    }

    /// Obszar roboczy: ekran minus dok po prawej.
    pub fn work_area(&self) -> Rect {
        Rect::new(
            0,
            0,
            self.screen.w.saturating_sub(self.theme.dock_width),
            self.screen.h,
        )
    }

    /// Wynik layoutu aktywnego obszaru roboczego (z logiki C++).
    pub fn placed_windows(&self) -> Vec<Placed> {
        self.logic.layout(self.work_area(), self.gap)
    }

    /// Okno pod punktem ekranu (pływające wygrywają), jeśli jest.
    pub fn window_at(&self, p: Point) -> Option<WindowId> {
        for pl in self.placed_windows().iter().rev() {
            if pl.rect.contains(p) {
                return Some(pl.id);
            }
        }
        None
    }

    /// Renderuje całą scenę do framebuffera `fb`.
    pub fn render(&self, fb: &mut [u32]) {
        crate::render::render_scene(self, fb);
    }
}
