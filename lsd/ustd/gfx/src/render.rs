//! Software rasterizer: pixels, fills, borders, lines, circles and 8x8 text.
//!
//! Everything works on a [`Canvas`], a raw view over the kernel-mapped
//! framebuffer. Coordinates are `i32` and every primitive clips to the buffer,
//! so drawing off the edge is a no-op rather than a fault.

use tgs_hal::ui::Framebuffer;

pub type Color = u32;

/// Pack an RGB triple into an XRGB8888 pixel (the framebuffer format).
pub const fn rgb(r: u8, g: u8, b: u8) -> Color {
    0xFF00_0000 | ((r as u32) << 16) | ((g as u32) << 8) | (b as u32)
}

/// A view over a kernel-mapped framebuffer.
#[derive(Clone, Copy)]
pub struct Canvas {
    pub base: *mut u32,
    pub width: u32,
    pub height: u32,
    /// Row stride in pixels (`>= width` when rows are padded).
    pub stride: u32,
    /// The kernel's 8x8 font: 96 glyphs of 8 bytes, index `ch - 32`.
    pub font: *const u8,
}

impl Canvas {
    pub fn from_framebuffer(fb: Framebuffer) -> Self {
        Canvas {
            base: fb.base,
            width: fb.width,
            height: fb.height,
            stride: fb.stride,
            font: fb.font,
        }
    }

    /// Write one pixel, clipped to the buffer.
    #[inline]
    pub fn put(&self, x: i32, y: i32, color: Color) {
        if x < 0 || y < 0 || (x as u32) >= self.width || (y as u32) >= self.height {
            return;
        }
        // SAFETY: the bounds check above keeps the index inside the mapped buffer.
        unsafe {
            self.base
                .add(y as usize * self.stride as usize + x as usize)
                .write_volatile(color);
        }
    }

    /// Read one pixel, clipped to the buffer (`0` outside).
    #[inline]
    pub fn get(&self, x: i32, y: i32) -> Color {
        if x < 0 || y < 0 || (x as u32) >= self.width || (y as u32) >= self.height {
            return 0;
        }
        // SAFETY: the bounds check above keeps the index inside the mapped buffer.
        unsafe {
            self.base
                .add(y as usize * self.stride as usize + x as usize)
                .read_volatile()
        }
    }
}

/// Open the framebuffer and wrap it in a [`Canvas`].
pub fn open_canvas() -> Option<Canvas> {
    tgs_hal::ui::ui_open().map(Canvas::from_framebuffer)
}

/// Fill a rectangle. Clipped to the buffer.
pub fn fill_rect(c: &Canvas, x: i32, y: i32, rw: u32, rh: u32, color: Color) {
    for row in 0..rh {
        let dy = y + row as i32;
        if dy < 0 || dy >= c.height as i32 {
            continue;
        }
        for col in 0..rw {
            let dx = x + col as i32;
            if dx < 0 || dx >= c.width as i32 {
                continue;
            }
            c.put(dx, dy, color);
        }
    }
}

/// Draw a 1-pixel rectangle border.
pub fn draw_rect(c: &Canvas, x: i32, y: i32, rw: u32, rh: u32, color: Color) {
    fill_rect(c, x, y, rw, 1, color);
    fill_rect(c, x, y + rh as i32 - 1, rw, 1, color);
    fill_rect(c, x, y, 1, rh, color);
    fill_rect(c, x + rw as i32 - 1, y, 1, rh, color);
}

/// Draw a line with Bresenham's algorithm.
pub fn draw_line(c: &Canvas, x0: i32, y0: i32, x1: i32, y1: i32, color: Color) {
    let mut x = x0;
    let mut y = y0;
    let dx = (x1 - x0).abs();
    let sx = if x0 < x1 { 1 } else { -1 };
    let dy = -((y1 - y0).abs());
    let sy = if y0 < y1 { 1 } else { -1 };
    let mut err = dx + dy;

    loop {
        c.put(x, y, color);
        if x == x1 && y == y1 {
            break;
        }
        let e2 = 2 * err;
        if e2 >= dy {
            err += dy;
            x += sx;
        }
        if e2 <= dx {
            err += dx;
            y += sy;
        }
    }
}

/// Draw a circle outline with the midpoint algorithm.
pub fn draw_circle(c: &Canvas, cx: i32, cy: i32, r: i32, color: Color) {
    let mut x = r;
    let mut y = 0;
    let mut err = 1 - r;
    while x >= y {
        c.put(cx + x, cy + y, color);
        c.put(cx + y, cy + x, color);
        c.put(cx - y, cy + x, color);
        c.put(cx - x, cy + y, color);
        c.put(cx - x, cy - y, color);
        c.put(cx - y, cy - x, color);
        c.put(cx + y, cy - x, color);
        c.put(cx + x, cy - y, color);
        y += 1;
        if err < 0 {
            err += 2 * y + 1;
        } else {
            x -= 1;
            err += 2 * (y - x) + 1;
        }
    }
}

/// Fill a disc.
pub fn fill_circle(c: &Canvas, cx: i32, cy: i32, r: i32, color: Color) {
    let r2 = r * r;
    for dy in -r..=r {
        for dx in -r..=r {
            if dx * dx + dy * dy <= r2 {
                c.put(cx + dx, cy + dy, color);
            }
        }
    }
}

/// Draw an ASCII string with the kernel's 8x8 console font.
///
/// The font at [`Canvas::font`] holds 96 glyphs of 8 bytes: index `ch - 32`
/// for `0x20..=0x7E`, index 95 (`?`) for anything else. Bit 7 of each row byte
/// is the leftmost pixel.
pub fn draw_text(c: &Canvas, x: i32, y: i32, text: &str, color: Color) {
    let mut cx = x;
    for ch in text.chars() {
        let code = ch as u32;
        let idx = if (0x20..=0x7E).contains(&code) {
            code - 0x20
        } else {
            95
        };
        for row in 0..8u32 {
            // SAFETY: `font` points at the kernel-mapped 768-byte font and
            // `idx` is always in `0..96`.
            let bits = unsafe { *c.font.add((idx * 8 + row) as usize) };
            for col in 0..8u32 {
                if bits & (0x80 >> col) != 0 {
                    c.put(cx + col as i32, y + row as i32, color);
                }
            }
        }
        cx += 8;
    }
}

/// A rainbow hue wheel; `h` advances by 256 per full turn. Returns XRGB.
///
/// Integer-only, so an animated app can cycle colours without linking `libm`.
pub fn hue(h: u32) -> Color {
    let i = (h & 0xFF) as i32;
    let r = clamp(if i < 128 { 0 } else if i < 192 { (i - 128) * 4 } else { 255 });
    let g = clamp(if i < 64 { i * 4 } else if i < 192 { 255 } else { 255 - (i - 192) * 4 });
    let b = clamp(if i < 64 { 255 } else if i < 128 { 255 - (i - 64) * 4 } else { 0 });
    rgb(r as u8, g as u8, b as u8)
}

fn clamp(v: i32) -> i32 {
    if v < 0 {
        0
    } else if v > 255 {
        255
    } else {
        v
    }
}
