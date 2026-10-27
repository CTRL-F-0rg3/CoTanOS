//! `i3dw` — a KDE-Plasma-like floating desktop environment for TrangorgeOS
//! userspace.
//!
//! Where `allde` is a *niri-like tiling* desktop, `i3dw`
//! is a *Plasma-like floating* desktop: a wallpaper gradient, a bottom panel
//! with a launcher and a system tray, floating windows with title bars and
//! close/maximize/minimize controls, and a Kickoff-style application menu.
//!
//! The desktop is a pure renderer over a `&mut [u32]` framebuffer, so it runs
//! unchanged on the host (the `i3dw` binary writes a PPM) and, once the ring-3
//! std runtime is wired, over the kernel-provided framebuffer.

pub mod apps;
pub mod cursor;
pub mod launcher;
pub mod panel;
pub mod theme;
pub mod wm;

#[cfg(test)]
mod tests;

use std::collections::HashMap;

use uspace::gfx::render::{draw_rect, fill_rect};
use uspace::gfx::text::{self, Align};

pub use apps::AppKind;
pub use cursor::DirtyCursor;
pub use wm::{Rect, WindowId, Wm};

/// The whole desktop environment.
pub struct I3dw {
    pub wm: Wm,
    pub screen_w: u32,
    pub screen_h: u32,
    /// The system-tray clock text, e.g. "14:32".
    pub clock: String,
    /// The date under the clock, e.g. "18 paź 2026".
    pub date: String,
    /// Whether the launcher popup is open.
    pub launcher_open: bool,
    /// The mouse pointer, drawn with a dirty region (only the cursor's box is
    /// redrawn on movement).
    pub cursor: DirtyCursor,
    apps: HashMap<WindowId, AppKind>,
}

impl I3dw {
    pub fn new(screen_w: u32, screen_h: u32) -> Self {
        Self {
            wm: Wm::new(screen_w, screen_h),
            screen_w,
            screen_h,
            clock: "00:00".to_string(),
            date: String::new(),
            launcher_open: false,
            cursor: DirtyCursor::new(screen_w, screen_h),
            apps: HashMap::new(),
        }
    }

    /// Spawn an app as a new floating window; returns its window id.
    pub fn spawn_app(&mut self, kind: AppKind) -> WindowId {
        let (w, h) = kind.default_size();
        let id = self.wm.add(kind.title(), w, h);
        self.apps.insert(id, kind);
        id
    }

    pub fn toggle_launcher(&mut self) {
        self.launcher_open = !self.launcher_open;
    }

    pub fn close_window(&mut self, id: WindowId) {
        self.wm.remove(id);
        self.apps.remove(&id);
    }

    /// Render a frame of the desktop into the framebuffer.
    pub fn render(&self, fb: &mut [u32]) {
        let w = self.screen_w;
        let h = self.screen_h;

        theme::fill_gradient(fb, w, h, theme::BG_TOP, theme::BG_BOTTOM);

        for (id, rect) in self.wm.layout(theme::PANEL_H) {
            self.render_window(fb, id, &rect);
        }

        panel::draw_panel(fb, w, h, &self.wm, self.launcher_open, &self.clock, &self.date);

        if self.launcher_open {
            launcher::draw_launcher(fb, w, h, &AppKind::all());
        }
    }

    /// Render the desktop and overlay the mouse cursor on top.
    ///
    /// The cursor is drawn *dirtily*: [`DirtyCursor`] remembers the pixels it
    /// covers, so later [`Self::move_cursor`] calls only touch the cursor's
    /// box instead of re-rendering the whole scene.
    pub fn render_live(&mut self, fb: &mut [u32]) {
        self.render(fb);
        self.cursor.attach(fb, self.screen_w, self.screen_h);
    }

    /// Move the cursor to `(x, y)`, redrawing only the dirty cursor region.
    pub fn move_cursor(&mut self, fb: &mut [u32], x: u32, y: u32) {
        self.cursor.move_to(fb, self.screen_w, self.screen_h, x, y);
    }

    /// Erase the cursor (e.g. before handing the framebuffer back).
    pub fn detach_cursor(&mut self, fb: &mut [u32]) {
        self.cursor.detach(fb, self.screen_w, self.screen_h);
    }


    fn render_window(&self, fb: &mut [u32], id: WindowId, r: &Rect) {
        let w = self.screen_w;
        let h = self.screen_h;
        let focused = self.wm.focused() == Some(id);

        fill_rect(fb, w, h, r.x, r.y, r.w, r.h, theme::WIN_BG);
        // Title bar.
        fill_rect(fb, w, h, r.x, r.y, r.w, theme::TITLE_H, theme::TITLE_BG);
        // Border, accent when focused.
        draw_rect(
            fb,
            w,
            h,
            r.x,
            r.y,
            r.w,
            r.h,
            if focused {
                theme::FOCUS_BORDER
            } else {
                theme::BORDER
            },
        );

        let title = self.apps.get(&id).map(|k| k.title()).unwrap_or("Okno");
        text::draw_clipped(
            fb, w, h, r.x + 8, r.y + 6, 14, title, theme::TEXT_DARK, r.w.saturating_sub(60), Align::Left,
        );
        draw_controls(fb, w, h, r);

        // Content area (inside the border, below the title bar).
        let cx = r.x + 1;
        let cy = r.y + theme::TITLE_H as i32 + 1;
        let cw = r.w.saturating_sub(2);
        let ch = r.h.saturating_sub(theme::TITLE_H + 2);
        if let Some(kind) = self.apps.get(&id) {
            apps::render_content(*kind, fb, w, h, cx, cy, cw, ch);
        }
    }
}

/// The three window controls (close / maximize / minimize) at the top-right.
fn draw_controls(fb: &mut [u32], w: u32, h: u32, r: &Rect) {
    let cy = r.y + (theme::TITLE_H as i32) / 2;
    let mut cx = r.x + r.w as i32 - 14;
    draw_dot(fb, w, h, cx, cy, theme::CLOSE);
    cx -= 18;
    draw_dot(fb, w, h, cx, cy, theme::MAXIMIZE);
    cx -= 18;
    draw_dot(fb, w, h, cx, cy, theme::MINIMIZE);
}

fn draw_dot(fb: &mut [u32], w: u32, h: u32, cx: i32, cy: i32, color: u32) {
    fill_rect(fb, w, h, cx - 5, cy - 5, 10, 10, color);
}
