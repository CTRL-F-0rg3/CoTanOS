//! `tgs-clock` — an analog clock on the ring-3 framebuffer.
//!
//! There is no `sin`/`cos` on this freestanding target (the `libm` stub is
//! empty), so the hands are positioned with a 16-entry quarter-sine table and
//! linear interpolation. The time is a software counter advanced once per
//! frame; on a real kernel the same loop would read `clock_gettime` instead.

// Link the `_start` entry point and `main` shim (not `use`d below).
extern crate tgs_rt;

use tgs_gfx::{draw_circle, draw_line, draw_text, fill_circle, fill_rect, idle, open_canvas, rgb, Canvas, Color};

/// `sin(i * 90/16)` scaled to 0..255, plus one extra sample at 90 degrees.
const QUARTER_SIN: [i32; 17] = [
    0, 27, 53, 79, 104, 127, 150, 171, 189, 206, 221, 233, 243, 249, 253, 255, 255,
];

/// Integer sine in `-255..=255` for an angle in degrees.
fn isin(deg: i32) -> i32 {
    let deg = deg.rem_euclid(360);
    let a = if deg < 90 {
        deg
    } else if deg < 180 {
        180 - deg
    } else if deg < 270 {
        deg - 180
    } else {
        360 - deg
    };
    // `a` is now in 0..=90 and `sin(a)` is non-negative.
    let seg = a * 16 / 90;
    let idx = seg.min(15) as usize;
    let frac = a * 16 - seg * 90;
    let v = QUARTER_SIN[idx] + (QUARTER_SIN[idx + 1] - QUARTER_SIN[idx]) * frac / 90;
    if deg < 180 {
        v
    } else {
        -v
    }
}

/// Integer cosine: `cos(a) = sin(a + 90)`.
fn icos(deg: i32) -> i32 {
    isin(deg + 90)
}

fn draw_hand(c: &Canvas, cx: i32, cy: i32, deg: i32, len: i32, color: Color) {
    let x = cx + (len * icos(deg)) / 256;
    let y = cy - (len * isin(deg)) / 256;
    draw_line(c, cx, cy, x, y, color);
}

fn main() {
    let Some(c) = open_canvas() else {
        idle();
    };

    let w = c.width as i32;
    let h = c.height as i32;
    let cx = w / 2;
    let cy = h / 2;
    let r = (w.min(h) / 2 - 16).max(8);

    let mut t: u32 = 0; // seconds of a software clock

    loop {
        fill_rect(&c, 0, 0, w as u32, h as u32, rgb(0x10, 0x12, 0x18));

        // Face.
        draw_circle(&c, cx, cy, r, rgb(0xE0, 0xE8, 0xF0));
        draw_circle(&c, cx, cy, r + 1, rgb(0xE0, 0xE8, 0xF0));

        // Hour ticks.
        for hh in 0..12 {
            let a = hh * 30;
            let x1 = cx + (r * icos(a)) / 256;
            let y1 = cy - (r * isin(a)) / 256;
            let x2 = cx + ((r - 12) * icos(a)) / 256;
            let y2 = cy - ((r - 12) * isin(a)) / 256;
            draw_line(&c, x1, y1, x2, y2, rgb(0x60, 0x70, 0x80));
        }

        // Hands.
        let sec = (t % 60) as i32;
        let min = ((t / 60) % 60) as i32;
        let hr = ((t / 3600) % 12) as i32;
        draw_hand(&c, cx, cy, hr * 30 + min / 2, r * 5 / 10, rgb(0xE0, 0xE8, 0xF0));
        draw_hand(&c, cx, cy, min * 6, r * 7 / 10, rgb(0xA0, 0xB0, 0xC0));
        draw_hand(&c, cx, cy, sec * 6, r * 8 / 10, rgb(0xFF, 0x50, 0x50));

        fill_circle(&c, cx, cy, 3, rgb(0xFF, 0xFF, 0xFF));

        draw_text(&c, cx - 44, cy + r / 2 + 8, "TRANGORGEOS", rgb(0x80, 0x90, 0xA0));

        t = t.wrapping_add(1);
        tgs_hal::gate::GATE.yield_now();
    }
}
