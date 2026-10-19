//! `titanus` — the interactive Titanus desktop, running in **userspace**.
//!
//! The security-correct home for the DE: it draws to the kernel-mapped
//! framebuffer obtained through `TGS_UI_OPEN` and reads the mouse/keyboard
//! through `TGS_MOUSE`/`TGS_KEY`, never touching hardware or privileged state
//! itself. The kernel only maps a buffer and forwards input packets.

use tgs_gfx::{draw_text, fill_rect, open_canvas, rgb, Canvas};
use tgs_hal::ui::{key_poll, mouse_poll};

const BG_TOP: u32 = 0xFF23273A;
const BG_BOTTOM: u32 = 0xFF0E0F1A;
const DOCK_BG: u32 = 0xFF181A28;
const ACCENT: u32 = 0xFF4F8CF7;
const WIN_BG: u32 = 0xFF1C1E2C;
const TITLE_BG: u32 = 0xFF272A3A;
const BORDER: u32 = 0xFF3A3E52;
const TEXT: u32 = 0xFFE6E8F0;
const WHITE: u32 = 0xFFFFFFFF;
const CLOSE: u32 = 0xFFED5A5A;
const PROMPT: u32 = 0xFF7CF09C;

fn lerp(a: u32, b: u32, t: u32) -> u32 {
    let t = t.min(255);
    let inv = 255 - t;
    let r = (((a >> 16) & 0xFF) * inv + ((b >> 16) & 0xFF) * t) / 255;
    let g = (((a >> 8) & 0xFF) * inv + ((b >> 8) & 0xFF) * t) / 255;
    let bl = ((a & 0xFF) * inv + (b & 0xFF) * t) / 255;
    0xFF00_0000 | (r << 16) | (g << 8) | bl
}

fn wallpaper(c: &Canvas) {
    for y in 0..c.height as i32 {
        let t = if c.height <= 1 { 0 } else { (y as u32 * 255) / (c.height - 1) };
        fill_rect(c, 0, y, c.width, 1, lerp(BG_TOP, BG_BOTTOM, t));
    }
}

fn draw_dock(c: &Canvas, w: i32, h: i32, dock_w: i32) {
    fill_rect(c, w - dock_w, 0, dock_w as u32, h as u32, DOCK_BG);
    draw_text(c, w - dock_w + 18, 12, "T", TEXT);
    draw_text(c, w - dock_w + 18, 44, "M", TEXT);
    draw_text(c, w - dock_w + 18, 76, "W", TEXT);
    draw_text(c, w - dock_w + 10, h - 24, "16:42", TEXT);
}

fn draw_window(c: &Canvas, x: i32, y: i32, w: u32, h: u32, title: &str, focused: bool) {
    let border = if focused { ACCENT } else { BORDER };
    fill_rect(c, x, y, w, h, border);
    fill_rect(c, x + 1, y + 1, w - 2, h - 2, WIN_BG);
    fill_rect(c, x + 1, y + 1, w - 2, 20, TITLE_BG);
    draw_text(c, x + 6, y + 6, title, TEXT);
    draw_text(c, x + w as i32 - 16, y + 6, "x", CLOSE);
    draw_text(c, x + 6, y + 28, "user@titanus:~$", PROMPT);
    draw_text(c, x + 6, y + 42, "GUI w userspace (ring 3)", rgb(0x8A, 0x8F, 0xA0));
}

// 16x16 arrow glyph (bit 15 = leftmost pixel).
const ARROW: [u16; 16] = [
    0x8000, 0xC000, 0xA000, 0x9000, 0x8800, 0x8400, 0x8200, 0x8100,
    0x8080, 0x8040, 0x8020, 0x8010, 0xF800, 0x8000, 0x8000, 0x8000,
];

fn draw_cursor(c: &Canvas, x: i32, y: i32) {
    for (row, bits) in ARROW.iter().enumerate() {
        for col in 0..16 {
            if bits & (0x8000 >> col) != 0 {
                c.put(x + col, y + row as i32, WHITE);
            }
        }
    }
}

fn save(c: &Canvas, x: i32, y: i32, buf: &mut [u32; 256]) {
    for row in 0..16 {
        for col in 0..16 {
            buf[row * 16 + col] = c.get(x + col as i32, y + row as i32);
        }
    }
}

fn restore(c: &Canvas, x: i32, y: i32, buf: &[u32; 256]) {
    for row in 0..16 {
        for col in 0..16 {
            c.put(x + col as i32, y + row as i32, buf[row * 16 + col]);
        }
    }
}

fn main() {
    let Some(c) = open_canvas() else {
        tgs_gfx::idle();
    };
    let w = c.width as i32;
    let h = c.height as i32;
    let dock_w = 56i32;

    // Static scene (drawn once).
    wallpaper(&c);
    draw_dock(&c, w, h, dock_w);

    let (wx, wy, ww, wh) = (60i32, 50i32, 420u32, 240u32);
    let mut focused = true;
    draw_window(&c, wx, wy, ww, wh, "Terminal", focused);

    // Dirty cursor.
    let (mut cx, mut cy) = (w / 2, h / 2);
    let mut bg = [0u32; 256];
    save(&c, cx, cy, &mut bg);
    draw_cursor(&c, cx, cy);

    let mut prev_btn = 0u32;

    loop {
        let mut moved = false;

        if let Some((dx, dy, btn)) = mouse_poll() {
            // PS/2 reports Y inverted.
            let (nx, ny) = ((cx + dx).clamp(0, w - 1), (cy - dy).clamp(0, h - 1));
            moved = nx != cx || ny != cy;
            if moved {
                restore(&c, cx, cy, &bg);
                cx = nx;
                cy = ny;
            }

            // Click toggles the window focus.
            if (btn & 1) != 0 && (prev_btn & 1) == 0 {
                let on_title = cx >= wx && cx < wx + ww as i32 && cy >= wy && cy < wy + 20;
                if on_title != focused {
                    focused = on_title;
                    // Redraw just the window (scene is otherwise static).
                    restore(&c, cx, cy, &bg);
                    draw_window(&c, wx, wy, ww, wh, "Terminal", focused);
                }
            }
            prev_btn = btn;
        }

        if let Some(k) = key_poll() {
            if k == '\x1b' {
                break; // ESC returns to the shell.
            }
        }

        if moved {
            save(&c, cx, cy, &mut bg);
            draw_cursor(&c, cx, cy);
        }

        tgs_hal::gate::GATE.yield_now();
    }
}
