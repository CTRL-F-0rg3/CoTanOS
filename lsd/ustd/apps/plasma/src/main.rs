//! `tgs-plasma` — an animated plasma on the ring-3 framebuffer.
//!
//! Purely integer: a radial ripple modulated by a diagonal XOR texture, mapped
//! through a rainbow hue wheel. It never needs `libm`, so it links on a
//! freestanding target where `sin`/`cos` are stubs.

// Link the `_start` entry point and `main` shim (not `use`d below).
extern crate tgs_rt;

use tgs_gfx::{hue, idle, open_canvas, Color};

fn main() {
    let Some(c) = open_canvas() else {
        idle();
    };

    let w = c.width as i32;
    let h = c.height as i32;
    let cx = w / 2;
    let cy = h / 2;

    // Precompute the palette so the inner loop is one table lookup.
    let palette = make_palette();

    let mut t: u32 = 0;
    loop {
        for y in 0..h {
            for x in 0..w {
                let dx = x - cx;
                let dy = y - cy;
                let d = ((dx * dx + dy * dy) >> 9) as u32;
                let v = (d.wrapping_sub(t).wrapping_add(((x as u32) ^ (y as u32)) >> 2)) & 0xFF;
                c.put(x, y, palette[v as usize]);
            }
        }
        t = t.wrapping_add(2);
        tgs_hal::gate::GATE.yield_now();
    }
}

fn make_palette() -> [Color; 256] {
    let mut pal = [0u32; 256];
    for (i, slot) in pal.iter_mut().enumerate() {
        *slot = hue(i as u32);
    }
    pal
}
