//! The bottom panel: a KDE-Plasma-style taskbar.
//!
//! Left: the launcher button (highlighted when the menu is open). Middle: one
//! task button per window, the focused one highlighted. Right: the system tray
//! with the clock and the date.

use uspace::gfx::render::fill_rect;
use uspace::gfx::text::{self, Align};

use crate::theme;
use crate::wm::Wm;

/// Draw the panel across the bottom of the screen.
pub fn draw_panel(
    fb: &mut [u32],
    sw: u32,
    sh: u32,
    wm: &Wm,
    launcher_open: bool,
    clock: &str,
    date: &str,
) {
    let py = (sh - theme::PANEL_H) as i32;
    fill_rect(fb, sw, sh, 0, py, sw, theme::PANEL_H, theme::PANEL_BG);
    // 1px top edge, the only "border" a Plasma panel has.
    fill_rect(fb, sw, sh, 0, py, sw, 1, theme::PANEL_EDGE);

    // Launcher button (left), highlighted while the menu is open.
    let btn = 40u32;
    let bx = 4;
    let by = py + 4;
    let btn_bg = if launcher_open {
        theme::ACCENT
    } else {
        theme::PANEL_TASK_BG
    };
    theme::fill_rounded_rect(fb, sw, sh, bx, by, btn, btn, 8, btn_bg);
    draw_grid_glyph(fb, sw, sh, bx, by, btn);

    // Task buttons (middle).
    let mut tx = bx + btn as i32 + 8;
    let focused = wm.focused();
    for w in wm.windows() {
        let task_w = 140u32;
        let task_h = 40u32;
        let bg = if focused == Some(w.id) {
            theme::PANEL_TASK_FOCUS
        } else {
            theme::PANEL_TASK_BG
        };
        theme::fill_rounded_rect(fb, sw, sh, tx, py + 4, task_w, task_h, 6, bg);
        text::draw_clipped(
            fb,
            sw,
            sh,
            tx + 10,
            py + 4 + 6,
            14,
            &w.title,
            theme::PANEL_TEXT,
            task_w - 20,
            Align::Left,
        );
        tx += task_w as i32 + 6;
    }

    // System tray (right): clock + date.
    let cw = text::measure(17, clock);
    let dw = text::measure(14, date);
    let tray_w = cw.max(dw) + 24;
    let tray_x = sw as i32 - tray_w as i32 - 8;
    theme::fill_rounded_rect(fb, sw, sh, tray_x, py + 4, tray_w, 40, 6, theme::PANEL_TASK_BG);
    text::draw_clipped(
        fb, sw, sh, tray_x + 12, py + 4 + 2, 17, clock, theme::PANEL_TEXT, tray_w - 24, Align::Left,
    );
    text::draw_clipped(
        fb, sw, sh, tray_x + 12, py + 4 + 24, 14, date, theme::PANEL_TEXT, tray_w - 24, Align::Left,
    );
}

/// A 2x2 grid of dots: the "application launcher" glyph.
fn draw_grid_glyph(fb: &mut [u32], sw: u32, sh: u32, bx: i32, by: i32, btn: u32) {
    let d = 6u32;
    let gap = 5u32;
    let total = 2 * d + gap;
    let off = (btn - total) as i32 / 2;
    for r in 0..2 {
        for c in 0..2 {
            let px = bx + off + c as i32 * (d + gap) as i32;
            let py = by + off + r as i32 * (d + gap) as i32;
            fill_rect(fb, sw, sh, px, py, d, d, theme::PANEL_TEXT);
        }
    }
}
