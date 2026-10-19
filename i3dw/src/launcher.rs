//! The application launcher (a Plasma "Kickoff"-like start menu).
//!
//! A popup above the panel listing the apps the desktop can spawn. Each row is
//! an accent icon tile with the app's initial, followed by the app name.

use uspace::gfx::render::fill_rect;
use uspace::gfx::text::{self, Align};

use crate::apps::AppKind;
use crate::theme;

/// Draw the launcher popup when open.
///
/// `apps` is the list of apps shown, in order.
pub fn draw_launcher(fb: &mut [u32], sw: u32, sh: u32, apps: &[AppKind]) {
    let popup_w = 300u32;
    let row_h = 30u32;
    let header_h = 40u32;
    let popup_h = header_h + apps.len() as u32 * row_h + 12;
    let px = 8;
    let py = (sh - theme::PANEL_H) as i32 - popup_h as i32 - 8;

    theme::fill_rounded_rect(fb, sw, sh, px, py, popup_w, popup_h, 10, theme::LAUNCHER_BG);

    // Header.
    text::draw_clipped(
        fb,
        sw,
        sh,
        px + 14,
        py + 10,
        17,
        "Aplikacje",
        theme::LAUNCHER_TEXT,
        popup_w - 28,
        Align::Left,
    );

    // Rows: an accent tile with the app's initial, then the name.
    let mut ry = py + header_h as i32;
    for kind in apps {
        let tile = 16u32;
        fill_rect(fb, sw, sh, px + 14, ry + 7, tile, tile, theme::LAUNCHER_ACCENT);
        let initial = kind
            .label()
            .chars()
            .next()
            .map(|c| c.to_string())
            .unwrap_or_default();
        theme::draw_centered(
            fb, sw, sh, px + 14, ry + 7, tile, tile, 14, &initial, theme::LAUNCHER_BG,
        );
        text::draw_clipped(
            fb,
            sw,
            sh,
            px + 40,
            ry + 2,
            14,
            kind.label(),
            theme::LAUNCHER_TEXT,
            popup_w - 54,
            Align::Left,
        );
        ry += row_h as i32;
    }
}
