//! `tgs-paint` — a bouncing brush that leaves a colour trail.
//!
//! The screen is *not* cleared between frames, so the trail accumulates the way
//! a paint program would. The hue cycles with each bounce step, giving the
//! scribble a rainbow gradient.

// Link the `_start` entry point and `main` shim (not `use`d below).
extern crate tgs_rt;

use tgs_gfx::{draw_circle, draw_text, fill_circle, fill_rect, hue, idle, open_canvas, rgb};

fn main() {
    let Some(c) = open_canvas() else {
        idle();
    };

    let w = c.width as i32;
    let h = c.height as i32;

    fill_rect(&c, 0, 0, w as u32, h as u32, rgb(0x0A, 0x0C, 0x10));
    draw_text(&c, 8, 8, "PAINT", rgb(0xFF, 0xFF, 0xFF));

    let radius = 10i32;
    let mut x = w / 2;
    let mut y = h / 2;
    let mut vx = 3i32;
    let mut vy = 2i32;
    let mut step: u32 = 0;

    loop {
        // Leave a coloured blob (the trail accumulates on the uncleared buffer).
        fill_circle(&c, x, y, radius, hue(step.wrapping_mul(3)));
        // The "cursor" outline.
        draw_circle(&c, x, y, radius, rgb(0xFF, 0xFF, 0xFF));

        // Advance and bounce off the edges.
        x += vx;
        y += vy;
        if x <= radius || x >= w - radius - 1 {
            vx = -vx;
            x += vx;
        }
        if y <= radius || y >= h - radius - 1 {
            vy = -vy;
            y += vy;
        }

        step = step.wrapping_add(1);
        tgs_hal::gate::GATE.yield_now();
    }
}
