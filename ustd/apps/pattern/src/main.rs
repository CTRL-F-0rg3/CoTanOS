//! `tgs-pattern` — a SMPTE-style test card on the ring-3 framebuffer.
//!
//! Static: it draws once, then parks in the kernel-yielding idle loop so the
//! image stays on screen and the world stays schedulable.

// Link the `_start` entry point and `main` shim (not `use`d below).
extern crate tgs_rt;

use tgs_gfx::{draw_rect, draw_text, fill_rect, idle, open_canvas, rgb, Color};

fn main() {
    let Some(c) = open_canvas() else {
        idle();
    };

    let w = c.width as i32;
    let h = c.height as i32;

    // 75% colour bars across the top two thirds.
    let bars: [(Color, &str); 7] = [
        (rgb(0xC0, 0xC0, 0xC0), "WHITE"),
        (rgb(0xC0, 0xC0, 0x00), "YELLOW"),
        (rgb(0x00, 0xC0, 0xC0), "CYAN"),
        (rgb(0x00, 0xC0, 0x00), "GREEN"),
        (rgb(0xC0, 0x00, 0xC0), "MAGENTA"),
        (rgb(0xC0, 0x00, 0x00), "RED"),
        (rgb(0x00, 0x00, 0xC0), "BLUE"),
    ];

    let bar_w = w / bars.len() as i32;
    for (i, (col, name)) in bars.iter().enumerate() {
        let x = i as i32 * bar_w;
        fill_rect(&c, x, 0, bar_w as u32, (h * 2 / 3) as u32, *col);
        draw_text(&c, x + 4, h - 24, name, rgb(0xE8, 0xE8, 0xE8));
    }

    // Grayscale ramp across the bottom third.
    for x in 0..w {
        let t = (x as u32 * 255 / w as u32) as u8;
        fill_rect(&c, x, h * 2 / 3, 1, (h / 3) as u32, rgb(t, t, t));
    }

    draw_text(&c, 8, 8, "TRANGORGEOS TEST CARD", rgb(0xFF, 0xFF, 0xFF));
    draw_rect(&c, 0, 0, w as u32, h as u32, rgb(0xFF, 0xFF, 0xFF));

    idle();
}
