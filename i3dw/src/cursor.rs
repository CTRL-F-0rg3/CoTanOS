//! A "dirty" mouse cursor: only the small region under the pointer is touched.
//!
//! # Why "dirty"
//!
//! A full desktop re-render on every mouse move would re-draw hundreds of
//! thousands of pixels for a 16×16 change. The dirty cursor instead remembers
//! the pixels it is standing on, and on a move restores that small rectangle to
//! its old position and re-draws the arrow at the new one. The rest of the
//! framebuffer is never touched. This is the same technique the in-kernel
//! `allde` desktop uses (`cursor_save` / `cursor_restore` / `draw_cursor`).

use uspace::gfx::render::{fill_rect, rgb};

/// Cursor size, in pixels.
pub const CURSOR_W: u32 = 16;
pub const CURSOR_H: u32 = 16;

/// A 16×16 arrow glyph, one row per `u16`, bit 15 = leftmost pixel.
///
/// A classic pointer: a diagonal head, a horizontal "wing", and a straight
/// tail down the left column.
const ARROW: [u16; 16] = [
    0x8000, 0xC000, 0xA000, 0x9000, 0x8800, 0x8400, 0x8200, 0x8100, //
    0x8080, 0x8040, 0x8020, 0x8010, 0xF800, 0x8000, 0x8000, 0x8000, //
];

const FILL: u32 = rgb(0xFF, 0xFF, 0xFF);
const OUTLINE: u32 = rgb(0x10, 0x10, 0x10);

/// A movable cursor with a remembered backing region.
pub struct DirtyCursor {
    pub x: u32,
    pub y: u32,
    /// The pixels under the cursor, so it can be erased without a full redraw.
    saved: Vec<u32>,
    /// Whether the cursor is currently drawn on the framebuffer.
    active: bool,
}

impl DirtyCursor {
    pub fn new(screen_w: u32, screen_h: u32) -> Self {
        Self {
            x: screen_w / 4,
            y: screen_h / 4,
            saved: vec![0u32; (CURSOR_W * CURSOR_H) as usize],
            active: false,
        }
    }

    /// Draw the cursor onto the framebuffer (saving what it covers first).
    pub fn attach(&mut self, fb: &mut [u32], sw: u32, sh: u32) {
        if self.active {
            return;
        }
        self.save(fb, sw, sh);
        draw_arrow(fb, sw, sh, self.x, self.y);
        self.active = true;
    }

    /// Erase the cursor, restoring the pixels it covered.
    pub fn detach(&mut self, fb: &mut [u32], sw: u32, sh: u32) {
        if !self.active {
            return;
        }
        self.restore(fb, sw, sh);
        self.active = false;
    }

    /// Move the cursor to `(nx, ny)`. Only the old and new cursor regions are
    /// touched; the rest of the framebuffer is left alone.
    pub fn move_to(&mut self, fb: &mut [u32], sw: u32, sh: u32, nx: u32, ny: u32) {
        let nx = nx.min(sw.saturating_sub(CURSOR_W));
        let ny = ny.min(sh.saturating_sub(CURSOR_H));

        if !self.active {
            self.x = nx;
            self.y = ny;
            self.attach(fb, sw, sh);
            return;
        }

        if nx == self.x && ny == self.y {
            return;
        }

        // Erase at the old position, then draw at the new one.
        self.restore(fb, sw, sh);
        self.x = nx;
        self.y = ny;
        self.save(fb, sw, sh);
        draw_arrow(fb, sw, sh, self.x, self.y);
    }

    fn save(&mut self, fb: &[u32], sw: u32, sh: u32) {
        for row in 0..CURSOR_H {
            let y = self.y + row;
            if y >= sh {
                break;
            }
            for col in 0..CURSOR_W {
                let x = self.x + col;
                if x >= sw {
                    break;
                }
                self.saved[(row * CURSOR_W + col) as usize] = fb[(y * sw + x) as usize];
            }
        }
    }

    fn restore(&self, fb: &mut [u32], sw: u32, sh: u32) {
        for row in 0..CURSOR_H {
            let y = self.y + row;
            if y >= sh {
                break;
            }
            for col in 0..CURSOR_W {
                let x = self.x + col;
                if x >= sw {
                    break;
                }
                fb[(y * sw + x) as usize] = self.saved[(row * CURSOR_W + col) as usize];
            }
        }
    }
}

/// Draw the arrow glyph at `(x, y)`: a black outline offset on all four sides,
/// then the white fill on top. The outline trick keeps the arrow visible on any
/// background without a second bitmap.
fn draw_arrow(fb: &mut [u32], sw: u32, sh: u32, x: u32, y: u32) {
    for (dx, dy) in [(0i32, 1i32), (0, -1), (1, 0), (-1, 0)] {
        draw_shape(fb, sw, sh, x as i32 + dx, y as i32 + dy, OUTLINE);
    }
    draw_shape(fb, sw, sh, x as i32, y as i32, FILL);
}

fn draw_shape(fb: &mut [u32], sw: u32, sh: u32, x: i32, y: i32, color: u32) {
    for (row, bits) in ARROW.iter().enumerate() {
        let py = y + row as i32;
        if py < 0 || py >= sh as i32 {
            continue;
        }
        for col in 0..CURSOR_W as i32 {
            if bits & (0x8000 >> col) != 0 {
                let px = x + col;
                if px >= 0 && px < sw as i32 {
                    fill_rect(fb, sw, sh, px, py, 1, 1, color);
                }
            }
        }
    }
}
