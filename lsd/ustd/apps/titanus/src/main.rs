//! `titanus` — the interactive Titanus desktop, running in **userspace**.
//!
//! Architecture: the kernel only maps a framebuffer (`TGS_UI_OPEN`) and hands
//! over *raw* PS/2 mouse bytes (`TGS_MOUSE_RAW`) plus keyboard chars
//! (`TGS_KEY`). Everything else — the mouse packet decode, the window manager,
//! the wireframe wallpapers and the dirty-region compositor — is ring-3 code.
//!
//! ## Features
//! * a userspace PS/2 mouse driver (`tgs_hal::mouse::Mouse`), not `mouse.c`
//! * tiled (master + stack) and floating layouts — `Tab`/`f` toggles
//! * wireframe wallpapers — `w` cycles mesh / grid / concentric
//! * a dirty-region compositor: only damaged rectangles are recomposited
//! * click-to-focus, drag-to-move (floating), `Esc` exits

// Link the `_start` entry point and `main` shim (not `use`d below).
extern crate tgs_rt;

use tgs_gfx::{open_canvas, rgb, Canvas};
use tgs_hal::mouse::Mouse;
use tgs_hal::ui::key_poll;

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
const WIRE: u32 = 0xFF3D4A6B;
const MUTED: u32 = rgb(0x8A, 0x8F, 0xA0);

const DOCK_W: i32 = 56;
const TITLE_H: i32 = 20;
const MAX_WIN: usize = 3;

// ── geometry ────────────────────────────────────────────────────────────────

#[derive(Clone, Copy, PartialEq)]
struct Rect {
    x: i32,
    y: i32,
    w: i32,
    h: i32,
}

impl Rect {
    const fn new(x: i32, y: i32, w: i32, h: i32) -> Self {
        Rect { x, y, w, h }
    }
    fn right(&self) -> i32 {
        self.x + self.w
    }
    fn bottom(&self) -> i32 {
        self.y + self.h
    }
    fn contains(&self, px: i32, py: i32) -> bool {
        px >= self.x && px < self.right() && py >= self.y && py < self.bottom()
    }
    fn union(&self, o: &Rect) -> Rect {
        let x = self.x.min(o.x);
        let y = self.y.min(o.y);
        Rect::new(
            x,
            y,
            self.right().max(o.right()) - x,
            self.bottom().max(o.bottom()) - y,
        )
    }
}

// ── dirty-region tracker ────────────────────────────────────────────────────

struct Dirty {
    r: Option<Rect>,
}

impl Dirty {
    const fn new() -> Self {
        Dirty { r: None }
    }
    fn add(&mut self, r: Rect) {
        self.r = Some(match self.r {
            Some(cur) => cur.union(&r),
            None => r,
        });
    }
    fn take(&mut self) -> Option<Rect> {
        self.r.take()
    }
}

// ── clip-aware raster (all scene drawing clips to the damage rect) ─────────

#[inline]
fn put(c: &Canvas, clip: &Rect, x: i32, y: i32, color: u32) {
    if x >= clip.x && x < clip.right() && y >= clip.y && y < clip.bottom() {
        c.put(x, y, color);
    }
}

fn fill(c: &Canvas, clip: &Rect, x: i32, y: i32, w: i32, h: i32, color: u32) {
    let x0 = x.max(clip.x);
    let y0 = y.max(clip.y);
    let x1 = (x + w).min(clip.right());
    let y1 = (y + h).min(clip.bottom());
    for yy in y0..y1 {
        for xx in x0..x1 {
            c.put(xx, yy, color);
        }
    }
}

fn text(c: &Canvas, clip: &Rect, x: i32, y: i32, s: &str, color: u32) {
    let mut cx = x;
    for ch in s.chars() {
        let code = ch as u32;
        let idx = if (0x20..=0x7E).contains(&code) {
            code - 0x20
        } else {
            95
        };
        for row in 0..8u32 {
            let bits = unsafe { *c.font.add((idx * 8 + row) as usize) };
            for col in 0..8u32 {
                if bits & (0x80 >> col) != 0 {
                    put(c, clip, cx + col as i32, y + row as i32, color);
                }
            }
        }
        cx += 8;
    }
}

// ── wallpaper + wireframes ──────────────────────────────────────────────────

fn lerp(a: u32, b: u32, t: u32) -> u32 {
    let t = t.min(255);
    let inv = 255 - t;
    let r = (((a >> 16) & 0xFF) * inv + ((b >> 16) & 0xFF) * t) / 255;
    let g = (((a >> 8) & 0xFF) * inv + ((b >> 8) & 0xFF) * t) / 255;
    let bl = ((a & 0xFF) * inv + (b & 0xFF) * t) / 255;
    0xFF00_0000 | (r << 16) | (g << 8) | bl
}

fn wallpaper(c: &Canvas, clip: &Rect) {
    for y in clip.y..clip.bottom() {
        let t = if c.height <= 1 {
            0
        } else {
            (y as u32 * 255) / (c.height - 1)
        };
        fill(c, clip, clip.x, y, clip.w, 1, lerp(BG_TOP, BG_BOTTOM, t));
    }
}

fn wireframe(c: &Canvas, clip: &Rect, style: u8) {
    let w = c.width as i32;
    let h = c.height as i32;
    match style % 3 {
        0 => mesh(c, clip, w, h),
        1 => grid(c, clip, w, h),
        _ => concentric(c, clip, w, h),
    }
}

fn mesh(c: &Canvas, clip: &Rect, w: i32, h: i32) {
    let mut k = -h;
    while k < w {
        for x in 0..w {
            put(c, clip, x, x + k, WIRE);
            put(c, clip, x, k - x, WIRE);
        }
        k += 32;
    }
}

fn grid(c: &Canvas, clip: &Rect, w: i32, h: i32) {
    let mut x = 0;
    while x < w {
        for y in 0..h {
            put(c, clip, x, y, WIRE);
        }
        x += 32;
    }
    let mut y = 0;
    while y < h {
        for x in 0..w {
            put(c, clip, x, y, WIRE);
        }
        y += 32;
    }
}

fn concentric(c: &Canvas, clip: &Rect, w: i32, h: i32) {
    let mut inset = 4;
    while inset < w / 2 && inset < h / 2 {
        fill(c, clip, inset, inset, w - 2 * inset, 1, WIRE);
        fill(c, clip, inset, h - inset - 1, w - 2 * inset, 1, WIRE);
        fill(c, clip, inset, inset, 1, h - 2 * inset, WIRE);
        fill(c, clip, w - inset - 1, inset, 1, h - 2 * inset, WIRE);
        inset += 28;
    }
}

// ── window manager data ─────────────────────────────────────────────────────

#[derive(Clone, Copy, PartialEq)]
enum Layout {
    Tiled,
    Floating,
}

struct Win {
    title: &'static str,
    body: &'static str,
    x: i32,
    y: i32,
    w: i32,
    h: i32,
}

struct State {
    wins: [Win; MAX_WIN],
    layout: Layout,
    wire: u8,
    focus: usize,
    drag: Option<usize>,
    drag_off: (i32, i32),
    dirty: Dirty,
}

fn title_rect(win: &Win) -> Rect {
    Rect::new(win.x, win.y, win.w, TITLE_H)
}

fn draw_dock(c: &Canvas, clip: &Rect, layout: Layout) {
    let w = c.width as i32;
    let h = c.height as i32;
    fill(c, clip, w - DOCK_W, 0, DOCK_W, h, DOCK_BG);
    text(c, clip, w - DOCK_W + 18, 12, "T", TEXT);
    text(c, clip, w - DOCK_W + 18, 44, "M", TEXT);
    text(c, clip, w - DOCK_W + 18, 76, "W", TEXT);
    text(c, clip, w - DOCK_W + 10, h - 24, "16:42", TEXT);
    let tag = if layout == Layout::Tiled { "[T]" } else { "[F]" };
    text(c, clip, w - DOCK_W + 10, h - 48, tag, ACCENT);
}

fn draw_window(c: &Canvas, clip: &Rect, win: &Win, focused: bool) {
    let border = if focused { ACCENT } else { BORDER };
    fill(c, clip, win.x, win.y, win.w, win.h, border);
    fill(c, clip, win.x + 1, win.y + 1, win.w - 2, win.h - 2, WIN_BG);
    fill(c, clip, win.x + 1, win.y + 1, win.w - 2, TITLE_H, TITLE_BG);
    text(c, clip, win.x + 6, win.y + 6, win.title, TEXT);
    text(c, clip, win.x + win.w - 14, win.y + 6, "x", CLOSE);
    text(c, clip, win.x + 6, win.y + 28, "user@titanus:~$", PROMPT);
    text(c, clip, win.x + 6, win.y + 42, win.body, MUTED);
}

fn tiled_layout(work: Rect, wins: &mut [Win]) {
    let n = wins.len();
    if n == 0 {
        return;
    }
    if n == 1 {
        wins[0].x = work.x;
        wins[0].y = work.y;
        wins[0].w = work.w;
        wins[0].h = work.h;
        return;
    }
    let gap = 4;
    let master_w = work.w * 3 / 5;
    let stack_w = work.w - master_w - gap;
    let stack_h = (work.h - gap * (n as i32 - 2)) / (n as i32 - 1);
    // Window 0 is the master; the rest stack on the right.
    wins[0].x = work.x;
    wins[0].y = work.y;
    wins[0].w = master_w - gap;
    wins[0].h = work.h;
    let mut sy = work.y;
    for i in 1..n {
        wins[i].x = work.x + master_w;
        wins[i].y = sy;
        wins[i].w = stack_w;
        wins[i].h = if i == n - 1 {
            work.y + work.h - sy
        } else {
            stack_h
        };
        sy += wins[i].h + gap;
    }
}

fn window_at(wins: &[Win], px: i32, py: i32) -> Option<usize> {
    for i in (0..wins.len()).rev() {
        let r = Rect::new(wins[i].x, wins[i].y, wins[i].w, wins[i].h);
        if r.contains(px, py) {
            return Some(i);
        }
    }
    None
}

fn composite(c: &Canvas, clip: &Rect, st: &State) {
    wallpaper(c, clip);
    wireframe(c, clip, st.wire);
    draw_dock(c, clip, st.layout);
    for (i, win) in st.wins.iter().enumerate() {
        draw_window(c, clip, win, i == st.focus);
    }
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
    let work = Rect::new(0, 0, w - DOCK_W, h);

    let wins = [
        Win { title: "Terminal", body: "GUI w userspace (ring 3)", x: 0, y: 0, w: 0, h: 0 },
        Win { title: "Files", body: "/home/user  (tiled)", x: 0, y: 0, w: 0, h: 0 },
        Win { title: "System", body: "uptime 00:42  load 0.2", x: 0, y: 0, w: 0, h: 0 },
    ];

    let mut st = State {
        wins,
        layout: Layout::Tiled,
        wire: 0,
        focus: 0,
        drag: None,
        drag_off: (0, 0),
        dirty: Dirty::new(),
    };
    tiled_layout(work, &mut st.wins);

    // Initial full composite.
    let full = Rect::new(0, 0, w, h);
    composite(&c, &full, &st);

    // Cursor (dirty save/restore).
    let (mut cx, mut cy) = (w / 2, h / 2);
    let mut bg = [0u32; 256];
    save(&c, cx, cy, &mut bg);
    draw_cursor(&c, cx, cy);

    let mut mouse = Mouse::new();
    let mut prev_btn = 0u32;

    loop {
        let mut moved = false;
        let (mut old_cx, mut old_cy) = (cx, cy);

        // ── mouse (userspace PS/2 driver) ──
        if let Some((dx, dy, btn)) = mouse.poll() {
            // PS/2 reports Y inverted.
            let (nx, ny) = ((cx + dx).clamp(0, w - 1), (cy - dy).clamp(0, h - 1));
            if nx != cx || ny != cy {
                moved = true;
                old_cx = cx;
                old_cy = cy;
                cx = nx;
                cy = ny;
            }

            let pressed = (btn & 1) != 0 && (prev_btn & 1) == 0;
            let released = (btn & 1) == 0 && (prev_btn & 1) != 0;

            if pressed {
                if let Some(i) = window_at(&st.wins, cx, cy) {
                    if i != st.focus {
                        st.focus = i;
                        st.dirty.add(Rect::new(0, 0, w, h));
                    }
                    if st.layout == Layout::Floating && title_rect(&st.wins[i]).contains(cx, cy) {
                        st.drag = Some(i);
                        st.drag_off = (cx - st.wins[i].x, cy - st.wins[i].y);
                    }
                }
            }
            if released {
                st.drag = None;
            }

            if let Some(i) = st.drag {
                let nx2 = (cx - st.drag_off.0).clamp(0, (work.w - st.wins[i].w).max(0));
                let ny2 = (cy - st.drag_off.1).clamp(0, (work.h - st.wins[i].h).max(0));
                if nx2 != st.wins[i].x || ny2 != st.wins[i].y {
                    st.dirty.add(Rect::new(st.wins[i].x, st.wins[i].y, st.wins[i].w, st.wins[i].h));
                    st.wins[i].x = nx2;
                    st.wins[i].y = ny2;
                    st.dirty.add(Rect::new(st.wins[i].x, st.wins[i].y, st.wins[i].w, st.wins[i].h));
                }
            }

            prev_btn = btn;
        }

        // ── keyboard ──
        if let Some(k) = key_poll() {
            match k {
                '\x1b' => break,
                '\t' | 'f' => {
                    st.layout = if st.layout == Layout::Tiled {
                        Layout::Floating
                    } else {
                        Layout::Tiled
                    };
                    if st.layout == Layout::Tiled {
                        tiled_layout(work, &mut st.wins);
                    }
                    st.dirty.add(Rect::new(0, 0, w, h));
                }
                'w' => {
                    st.wire = st.wire.wrapping_add(1);
                    st.dirty.add(Rect::new(0, 0, w, h));
                }
                _ => {}
            }
        }

        // ── reconcile cursor + damage ──
        if moved {
            restore(&c, old_cx, old_cy, &bg);
        }
        if let Some(r) = st.dirty.take() {
            if !moved {
                restore(&c, cx, cy, &bg);
            }
            composite(&c, &r, &st);
            moved = true;
        }
        if moved {
            save(&c, cx, cy, &mut bg);
            draw_cursor(&c, cx, cy);
        }

        tgs_hal::gate::GATE.yield_now();
    }
}
