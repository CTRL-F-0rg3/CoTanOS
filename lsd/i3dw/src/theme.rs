//! KDE-Plasma-like palette and drawing helpers (Breeze-inspired).
//!
//! # Why a separate file
//!
//! The desktop *logic* (window manager, panel, launcher) must not know colours;
//! it asks the theme. That is the same split KDE uses: a `Plasma Theme` is data,
//! and the shell is code. Change these constants to restyle the whole desktop.
//!
//! The palette follows Plasma's "Breeze" defaults: a light window on a dark
//! panel, with the signature Breeze blue accent.

use uspace::gfx::render::{fill_rect, Color, rgb};
use uspace::gfx::text;

// ── wallpaper ────────────────────────────────────────────────────────────────

/// Top of the wallpaper gradient (deep Breeze blue).
pub const BG_TOP: Color = rgb(0x1F, 0x3B, 0x73);
/// Bottom of the wallpaper gradient.
pub const BG_BOTTOM: Color = rgb(0x0D, 0x1B, 0x3E);

// ── panel (Plasma Breeze dark) ───────────────────────────────────────────────

pub const PANEL_BG: Color = rgb(0x23, 0x23, 0x2E);
pub const PANEL_EDGE: Color = rgb(0x3D, 0x3D, 0x4A);
pub const PANEL_TEXT: Color = rgb(0xEF, 0xF0, 0xF1);
/// An inactive task button on the panel.
pub const PANEL_TASK_BG: Color = rgb(0x33, 0x33, 0x40);
/// A focused task button.
pub const PANEL_TASK_FOCUS: Color = rgb(0x3D, 0xAE, 0xE9);
/// Panel height, in pixels.
pub const PANEL_H: u32 = 48;

// ── window (Breeze light) ────────────────────────────────────────────────────

pub const WIN_BG: Color = rgb(0xFC, 0xFC, 0xFC);
pub const TITLE_BG: Color = rgb(0xEF, 0xF0, 0xF1);
pub const BORDER: Color = rgb(0xB8, 0xBC, 0xC2);
pub const FOCUS_BORDER: Color = rgb(0x3D, 0xAE, 0xE9);
pub const TEXT_DARK: Color = rgb(0x23, 0x26, 0x29);
pub const TEXT_MUTED: Color = rgb(0x6E, 0x75, 0x7D);
/// Height of the window title bar.
pub const TITLE_H: u32 = 28;

// ── accent & window controls ─────────────────────────────────────────────────

pub const ACCENT: Color = rgb(0x3D, 0xAE, 0xE9);
/// Close button.
pub const CLOSE: Color = rgb(0xED, 0x15, 0x15);
/// Minimize button.
pub const MINIMIZE: Color = rgb(0xF5, 0xC2, 0x11);
/// Maximize button.
pub const MAXIMIZE: Color = rgb(0x2E, 0xCC, 0x71);

// ── launcher ─────────────────────────────────────────────────────────────────

pub const LAUNCHER_BG: Color = rgb(0x2A, 0x2A, 0x35);
pub const LAUNCHER_EDGE: Color = rgb(0x3D, 0x3D, 0x4A);
pub const LAUNCHER_TEXT: Color = rgb(0xEF, 0xF0, 0xF1);
pub const LAUNCHER_ACCENT: Color = rgb(0x3D, 0xAE, 0xE9);

// ── helpers ──────────────────────────────────────────────────────────────────

/// Blend `a` → `b` by `t` in 0..=255.
#[inline]
pub fn lerp(a: Color, b: Color, t: u32) -> Color {
    let t = t.min(255);
    let inv = 255 - t;
    let r = (((a >> 16) & 0xFF) * inv + ((b >> 16) & 0xFF) * t) / 255;
    let g = (((a >> 8) & 0xFF) * inv + ((b >> 8) & 0xFF) * t) / 255;
    let bl = ((a & 0xFF) * inv + (b & 0xFF) * t) / 255;
    0xFF00_0000 | (r << 16) | (g << 8) | bl
}

/// Fill the whole buffer with a vertical gradient `top` → `bottom`.
pub fn fill_gradient(buf: &mut [u32], w: u32, h: u32, top: Color, bottom: Color) {
    for y in 0..h {
        let t = if h <= 1 { 0 } else { y * 255 / (h - 1) };
        let c = lerp(top, bottom, t);
        fill_rect(buf, w, h, 0, y as i32, w, 1, c);
    }
}

/// Integer square root, rounded down. Small radii only, so a subtract loop is
/// cheaper than floats or a lookup table.
fn isqrt(value: u32) -> u32 {
    if value == 0 {
        return 0;
    }
    let mut guess = 1u32;
    while (guess + 1).saturating_mul(guess + 1) <= value {
        guess += 1;
    }
    guess
}

/// Fill a rectangle with rounded corners, clipped to the buffer.
///
/// `radius` is clamped to half the shorter side, so a radius larger than
/// possible yields a capsule instead of a hole.
pub fn fill_rounded_rect(
    buf: &mut [u32],
    w: u32,
    h: u32,
    x: i32,
    y: i32,
    rw: u32,
    rh: u32,
    radius: u32,
    c: Color,
) {
    if rw == 0 || rh == 0 {
        return;
    }
    let radius = radius.min(rw.min(rh) / 2);

    for row in 0..rh {
        let py = y + row as i32;
        // Distance from the top/bottom edge of the rounded corner band.
        let into_corner = if row < radius {
            radius - row
        } else if row >= rh - radius {
            row - (rh - radius) + 1
        } else {
            0
        };

        let inset = if into_corner == 0 {
            0
        } else {
            // The circle equation `dx^2 + dy^2 = r^2`, solved for `dx`.
            let dy = into_corner - 1;
            let squared = radius * radius - dy * dy;
            radius - isqrt(squared)
        };

        let x0 = x + inset as i32;
        let x1 = x + rw as i32 - inset as i32;
        if x1 > x0 {
            fill_rect(buf, w, h, x0, py, (x1 - x0) as u32, 1, c);
        }
    }
}

/// Draw text centred vertically and horizontally inside a field.
///
/// Returns the width of the drawn text (from the font metrics) so callers can
/// lay out subsequent elements without re-measuring.
pub fn draw_centered(
    buf: &mut [u32],
    w: u32,
    h: u32,
    x: i32,
    y: i32,
    rw: u32,
    rh: u32,
    size: u32,
    text: &str,
    color: Color,
) -> u32 {
    let line_height = text::ascent(size).unwrap_or(size) + text::descent(size).unwrap_or(0);
    let top = y + ((rh as i32 - line_height as i32) / 2).max(0);
    text::draw_clipped(buf, w, h, x, top, size, text, color, rw, text::Align::Center)
}
