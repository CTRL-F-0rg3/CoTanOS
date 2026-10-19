//! Interactive `i3dw` desktop: a KDE-Plasma-like floating environment that
//! runs in kernel space and takes over input from the system terminal.
//!
//! Unlike `allde` (niri-like tiling), `i3dw` floats windows, stacks them in a
//! z-order, and draws a Plasma-style bottom panel (launcher + task list +
//! clock). The framebuffer comes from the vgpu/HDMI driver through
//! [`crate::gfx::console::try_fb`].
//!
//! Keys: arrows move the cursor, F4 clicks (focus/raise), Tab focuses next,
//! F1 minimizes, F2 closes, F3 opens a window, ESC returns to the terminal.
//! Typing goes into the focused window's terminal.

use alloc::string::{String, ToString};
use alloc::vec::Vec;

use crate::gfx::font::FONT8X8;
use crate::gfx::framebuffer::{rgb, Framebuffer};

extern "C" {
    fn mouse_init();
    fn mouse_poll(dx: *mut i32, dy: *mut i32, dz: *mut i32, buttons: *mut i32) -> i32;
}

const PROMPT: &str = "user@i3dw:~$ ";
const PANEL_H: u32 = 32;
const TITLE_H: u32 = 18;
const CURSOR_W: u32 = 16;
const CURSOR_H: u32 = 16;

const KEY_ENTER: u32 = 0x100;
const KEY_BACKSPACE: u32 = 0x101;
const KEY_ESC: u32 = 0x102;
const KEY_RIGHT: u32 = 0x103;
const KEY_LEFT: u32 = 0x104;
const KEY_DOWN: u32 = 0x105;
const KEY_UP: u32 = 0x106;
const KEY_TAB: u32 = 0x10A;
const KEY_F1: u32 = 0x112;
const KEY_F2: u32 = 0x113;
const KEY_F3: u32 = 0x114;

// ── Breeze palette ────────────────────────────────────────────────────────────
const BG_TOP: u32 = 0xFF1F3B73;
const BG_BOTTOM: u32 = 0xFF0D1B3E;
const PANEL_BG: u32 = 0xFF23232E;
const PANEL_EDGE: u32 = 0xFF3D3D4A;
const PANEL_TEXT: u32 = 0xFFEFF0F1;
const TASK_BG: u32 = 0xFF333340;
const ACCENT: u32 = 0xFF3DAEE9;
const WIN_BG: u32 = 0xFFFCFCFC;
const TITLE_BG: u32 = 0xFFEFF0F1;
const BORDER: u32 = 0xFFB8BCC2;
const FOCUS_BORDER: u32 = 0xFF3DAEE9;
const TEXT_DARK: u32 = 0xFF232629;
const TEXT_MUTED: u32 = 0xFF6E757D;
const TERM_BG: u32 = 0xFF1E1E28;
const TERM_PROMPT: u32 = 0xFF7CF09C;
const TERM_TEXT: u32 = 0xFFD0D8E0;
const CLOSE: u32 = 0xFFED1515;
const MAXIMIZE: u32 = 0xFF2ECC71;
const MINIMIZE: u32 = 0xFFF5C211;
const LAUNCHER_BG: u32 = 0xFF2A2A35;
const LAUNCHER_TEXT: u32 = 0xFFEFF0F1;

/// A 16×16 arrow glyph, one row per u16, bit 15 = leftmost pixel.
const ARROW: [u16; 16] = [
    0x8000, 0xC000, 0xA000, 0x9000, 0x8800, 0x8400, 0x8200, 0x8100, //
    0x8080, 0x8040, 0x8020, 0x8010, 0xF800, 0x8000, 0x8000, 0x8000, //
];

fn set(fb: &mut Framebuffer, x: u32, y: u32, c: u32) {
    if x < fb.width as u32 && y < fb.height as u32 {
        fb.set(x as usize, y as usize, c);
    }
}

fn get(fb: &Framebuffer, x: u32, y: u32) -> u32 {
    if x < fb.width as u32 && y < fb.height as u32 {
        fb.get(x as usize, y as usize)
    } else {
        0
    }
}

fn fill(fb: &mut Framebuffer, x: u32, y: u32, w: u32, h: u32, c: u32) {
    for yy in 0..h {
        for xx in 0..w {
            set(fb, x + xx, y + yy, c);
        }
    }
}

fn text(fb: &mut Framebuffer, x: u32, y: u32, s: &str, c: u32) {
    let mut cx = x;
    for &b in s.as_bytes() {
        if (32..=126).contains(&b) {
            let glyph = &FONT8X8[(b - 32) as usize];
            for (row, bits) in glyph.iter().enumerate() {
                for col in 0..8 {
                    if bits & (0x80 >> col) != 0 {
                        set(fb, cx + col, y + row as u32, c);
                    }
                }
            }
        }
        cx += 8;
    }
}

/// Vertical gradient across the whole framebuffer (the wallpaper).
fn wallpaper(fb: &mut Framebuffer) {
    let h = fb.height as u32;
    let w = fb.width as u32;
    for y in 0..h {
        let t = if h <= 1 { 0 } else { y * 255 / (h - 1) };
        let inv = 255 - t;
        let r = (((BG_TOP >> 16) & 0xFF) * inv + ((BG_BOTTOM >> 16) & 0xFF) * t) / 255;
        let g = (((BG_TOP >> 8) & 0xFF) * inv + ((BG_BOTTOM >> 8) & 0xFF) * t) / 255;
        let b = ((BG_TOP & 0xFF) * inv + (BG_BOTTOM & 0xFF) * t) / 255;
        fill(fb, 0, y, w, 1, rgb(r, g, b));
    }
}

/// Restore the wallpaper over one rectangle (dirty paper).
fn wallpaper_rect(fb: &mut Framebuffer, x: u32, y: u32, w: u32, h: u32) {
    let full_h = fb.height as u32;
    for row in 0..h {
        let gy = y + row;
        if gy >= full_h {
            break;
        }
        let t = if full_h <= 1 { 0 } else { gy * 255 / (full_h - 1) };
        let inv = 255 - t;
        let r = (((BG_TOP >> 16) & 0xFF) * inv + ((BG_BOTTOM >> 16) & 0xFF) * t) / 255;
        let g = (((BG_TOP >> 8) & 0xFF) * inv + ((BG_BOTTOM >> 8) & 0xFF) * t) / 255;
        let b = ((BG_TOP & 0xFF) * inv + (BG_BOTTOM & 0xFF) * t) / 255;
        fill(fb, x, gy, w, 1, rgb(r, g, b));
    }
}

/// Draw the arrow glyph at `(x, y)`: a black outline offset on four sides, then
/// the white fill on top. The outline trick keeps the arrow visible on any
/// background without a second bitmap.
fn draw_cursor(fb: &mut Framebuffer, x: u32, y: u32) {
    for (dx, dy) in [(0i32, 1i32), (0, -1), (1, 0), (-1, 0)] {
        draw_shape(fb, x as i32 + dx, y as i32 + dy, rgb(0x10, 0x10, 0x10));
    }
    draw_shape(fb, x as i32, y as i32, rgb(0xFF, 0xFF, 0xFF));
}

fn draw_shape(fb: &mut Framebuffer, x: i32, y: i32, c: u32) {
    for (row, bits) in ARROW.iter().enumerate() {
        let py = y + row as i32;
        if py < 0 || py >= fb.height as i32 {
            continue;
        }
        for col in 0..CURSOR_W as i32 {
            if bits & (0x8000 >> col) != 0 {
                let px = x + col;
                if px >= 0 && px < fb.width as i32 {
                    set(fb, px as u32, py as u32, c);
                }
            }
        }
    }
}

/// Remember the scene pixels under the cursor so they can be restored later.
fn cursor_save(fb: &Framebuffer, x: u32, y: u32, buf: &mut [u32]) {
    for yy in 0..CURSOR_H {
        for xx in 0..CURSOR_W {
            buf[(yy * CURSOR_W + xx) as usize] = get(fb, x + xx, y + yy);
        }
    }
}

fn cursor_restore(fb: &mut Framebuffer, x: u32, y: u32, buf: &[u32]) {
    for yy in 0..CURSOR_H {
        for xx in 0..CURSOR_W {
            set(fb, x + xx, y + yy, buf[(yy * CURSOR_W + xx) as usize]);
        }
    }
}


#[derive(Clone, Copy, PartialEq)]
enum Kind {
    Welcome,
    Terminal,
    Monitor,
}

impl Kind {
    fn title(self) -> &'static str {
        match self {
            Kind::Welcome => "Witaj",
            Kind::Terminal => "Terminal",
            Kind::Monitor => "Monitor systemu",
        }
    }
}

struct Window {
    kind: Kind,
    title: String,
    x: u32,
    y: u32,
    w: u32,
    h: u32,
    z: u32,
    minimized: bool,
    /// Set when this window's content or border changed and must be repainted.
    dirty: bool,
    lines: Vec<String>,
    input: String,
}

impl Window {
    fn new(kind: Kind, x: u32, y: u32, w: u32, h: u32, z: u32) -> Self {
        let mut win = Self {
            kind,
            title: kind.title().to_string(),
            x,
            y,
            w,
            h,
            z,
            minimized: false,
            dirty: true,
            lines: Vec::new(),
            input: String::new(),
        };
        if kind == Kind::Terminal {
            win.lines.push("TrangorgeOS i3dw".into());
            win.lines.push("type 'help' for commands".into());
        }
        win
    }
}

struct Plasma {
    windows: Vec<Window>,
    focused: usize,
    next_z: u32,
    cursor_x: u32,
    cursor_y: u32,
    launcher_open: bool,
    /// Whether the panel/launcher widget must be repainted.
    panel_dirty: bool,
    /// Rectangles where a window was removed/minimized, so the wallpaper must be
    /// restored there (dirty paper).
    paper: Vec<(u32, u32, u32, u32)>,
}

impl Plasma {
    fn new(sw: u32, sh: u32) -> Self {
        let mut d = Self {
            windows: Vec::new(),
            focused: 0,
            next_z: 1,
            cursor_x: sw / 2,
            cursor_y: sh / 3,
            launcher_open: false,
            panel_dirty: true,
            paper: Vec::new(),
        };
        // Cascade three floating windows in the Plasma style.
        let (ww, wh) = (400u32, 260u32);
        let (tw, th) = (520u32, 340u32);
        let (mw, mh) = (380u32, 240u32);
        let mx = sw.saturating_sub(tw) / 2;
        let my = sh.saturating_sub(PANEL_H).saturating_sub(th) / 2;
        d.add_window(Kind::Welcome, 40, 40, ww, wh);
        d.add_window(Kind::Terminal, mx, my, tw, th);
        d.add_window(Kind::Monitor, sw - mw - 40, 40, mw, mh);
        d
    }

    fn add_window(&mut self, kind: Kind, x: u32, y: u32, w: u32, h: u32) {
        let z = self.next_z;
        self.next_z += 1;
        self.windows.push(Window::new(kind, x, y, w, h, z));
        self.focused = self.windows.len() - 1;
        self.panel_dirty = true;
    }

    fn close_focused(&mut self) {
        if self.focused < self.windows.len() {
            let w = &self.windows[self.focused];
            self.paper.push((w.x, w.y, w.w, w.h));
            self.windows.remove(self.focused);
        }
        if self.windows.is_empty() {
            self.add_window(Kind::Terminal, 60, 60, 480, 300);
        }
        if self.focused >= self.windows.len() {
            self.focused = self.windows.len() - 1;
        }
        self.panel_dirty = true;
    }

    fn minimize_focused(&mut self) {
        if self.focused < self.windows.len() {
            let w = &mut self.windows[self.focused];
            w.minimized = !w.minimized;
            if w.minimized {
                self.paper.push((w.x, w.y, w.w, w.h));
            } else {
                w.dirty = true;
            }
        }
        self.panel_dirty = true;
    }

    fn focus_next(&mut self) {
        let n = self.windows.len();
        if n == 0 {
            return;
        }
        let old = self.focused;
        for step in 1..=n {
            let idx = (self.focused + step) % n;
            if !self.windows[idx].minimized {
                self.focused = idx;
                let z = self.next_z;
                self.next_z += 1;
                self.windows[idx].z = z;
                break;
            }
        }
        // Only the two borders and the panel widget changed.
        if old < self.windows.len() && old != self.focused {
            self.windows[old].dirty = true;
        }
        self.windows[self.focused].dirty = true;
        self.panel_dirty = true;
    }

    /// Raise the focused window to the top.
    fn raise(&mut self, idx: usize) {
        let z = self.next_z;
        self.next_z += 1;
        self.windows[idx].z = z;
    }

    /// Topmost non-minimized window under `(x, y)`.
    fn window_at(&self, x: u32, y: u32) -> Option<usize> {
        let mut best: Option<usize> = None;
        let mut best_z = 0u32;
        for (i, w) in self.windows.iter().enumerate() {
            if w.minimized {
                continue;
            }
            if x >= w.x && x < w.x + w.w && y >= w.y && y < w.y + w.h {
                if best.is_none() || w.z > best_z {
                    best = Some(i);
                    best_z = w.z;
                }
            }
        }
        best
    }

    fn click(&mut self, sh: u32) {
        let x = self.cursor_x;
        let y = self.cursor_y;
        let panel_y = sh.saturating_sub(PANEL_H);

        // Launcher button (bottom-left).
        if x >= 4 && x < 28 && y >= panel_y + 4 && y < panel_y + 28 {
            let was_open = self.launcher_open;
            self.launcher_open = !self.launcher_open;
            self.panel_dirty = true;
            if was_open {
                let py = panel_y.saturating_sub(4 + 3 * 26 + 28);
                self.paper.push((8, py, 220, 4 + 3 * 26 + 24));
            }
            return;
        }

        // Launcher popup (when open): a row per app above the panel.
        if self.launcher_open {
            let px = 8u32;
            let py = panel_y.saturating_sub(4 + 3 * 26 + 28);
            if x >= px && x < px + 220 && y >= py && y < panel_y {
                let row = (y - py) / 26;
                match row {
                    0 => self.add_window(Kind::Terminal, 60, 60, 480, 300),
                    1 => self.add_window(Kind::Monitor, 80, 80, 380, 240),
                    2 => self.add_window(Kind::Welcome, 100, 100, 400, 260),
                    _ => {}
                }
                self.launcher_open = false;
                self.panel_dirty = true;
                self.paper.push((px, py, 220, 4 + 3 * 26 + 24));
                return;
            }
        }

        // Task buttons on the panel.
        let mut tx = 36u32;
        for i in 0..self.windows.len() {
            if x >= tx && x < tx + 120 && y >= panel_y + 4 && y < panel_y + 28 {
                self.focus_to(i);
                return;
            }
            tx += 124;
        }

        // Windows (topmost first).
        if let Some(i) = self.window_at(x, y) {
            self.focus_to(i);
        }
    }

    /// Focus `idx`, repainting only the two affected windows and the panel.
    fn focus_to(&mut self, idx: usize) {
        if idx >= self.windows.len() {
            return;
        }
        self.windows[idx].minimized = false;
        let old = self.focused;
        self.focused = idx;
        self.raise(idx);
        if old != idx && old < self.windows.len() {
            self.windows[old].dirty = true;
        }
        self.windows[idx].dirty = true;
        self.panel_dirty = true;
    }

    fn type_char(&mut self, c: char) {
        if self.focused >= self.windows.len() {
            return;
        }
        match c {
            '\x08' => {
                self.windows[self.focused].input.pop();
            }
            c if (c as u32) >= 0x20 && (c as u32) < 0x7F => {
                self.windows[self.focused].input.push(c);
            }
            _ => {}
        }
        if c == '\n' {
            self.submit();
        }
        if self.focused < self.windows.len() {
            self.windows[self.focused].dirty = true;
        }
    }

    fn submit(&mut self) {
        if self.focused >= self.windows.len() {
            return;
        }
        let line = core::mem::take(&mut self.windows[self.focused].input);
        let (cmd, rest) = split_space(line.trim());

        match cmd {
            "new" => {
                self.windows[self.focused].lines.push(format!("{}{}", PROMPT, line));
                self.windows[self.focused].lines.push("spawned a new terminal".into());
                self.add_window(Kind::Terminal, 80, 80, 480, 300);
                return;
            }
            "exit" => {
                self.close_focused();
                return;
            }
            "list" => {
                let mut out = vec![format!("{}{}", PROMPT, line)];
                for (i, win) in self.windows.iter().enumerate() {
                    let mark = if i == self.focused { "*" } else { " " };
                    out.push(format!("{} [{}] {}", mark, i, win.title));
                }
                self.windows[self.focused].lines.extend(out);
                return;
            }
            _ => {}
        }

        let out: Vec<String> = match cmd {
            "help" => vec![
                "commands: help echo clear uname new list exit".into(),
                "keys: arrows=cursor F4=click Tab=focus".into(),
                "      F1=minimize F2=close F3=new ESC=quit".into(),
            ],
            "echo" => vec![rest.to_string()],
            "clear" => {
                self.windows[self.focused].lines.clear();
                vec![]
            }
            "uname" => vec!["TrangorgeOS i3dw (Plasma-style)".into()],
            _ => vec![format!("{}: command not found (try 'help')", cmd)],
        };
        self.windows[self.focused].lines.push(format!("{}{}", PROMPT, line));
        self.windows[self.focused].lines.extend(out);
    }

    fn handle_keycode(&mut self, k: u32) -> bool {
        match k {
            KEY_ESC => return true,
            KEY_ENTER => self.type_char('\n'),
            KEY_BACKSPACE => self.type_char('\x08'),
            KEY_TAB => self.focus_next(),
            KEY_F1 => self.minimize_focused(),
            KEY_F2 => self.close_focused(),
            KEY_F3 => self.add_window(Kind::Terminal, 80, 80, 480, 300),
            KEY_RIGHT => self.cursor_x = self.cursor_x.saturating_add(20),
            KEY_LEFT => self.cursor_x = self.cursor_x.saturating_sub(20),
            KEY_UP => self.cursor_y = self.cursor_y.saturating_sub(20),
            KEY_DOWN => self.cursor_y = self.cursor_y.saturating_add(20),
            _ => {
                if k < 0x80 {
                    self.type_char(k as u8 as char);
                }
            }
        }
        false
    }
}

fn split_space(s: &str) -> (&str, &str) {
    match s.find(' ') {
        Some(i) => (&s[..i], &s[i + 1..].trim()),
        None => (s, ""),
    }
}

impl Plasma {
    fn render_scene(&self, fb: &mut Framebuffer) {
        wallpaper(fb);

        // Floating windows, back-to-front by z.
        let mut order: Vec<usize> = (0..self.windows.len())
            .filter(|&i| !self.windows[i].minimized)
            .collect();
        order.sort_by_key(|&i| self.windows[i].z);
        for &i in &order {
            self.render_window(fb, i);
        }

        self.draw_panel(fb);
        if self.launcher_open {
            self.draw_launcher(fb);
        }
    }

    /// Repaint only the dirty regions: the wallpaper under removed/minimized
    /// windows (dirty paper), the windows whose content changed (dirty
    /// windows), and the panel widget (dirty widget). Nothing else is touched.
    fn apply_dirty(&mut self, fb: &mut Framebuffer) {
        // Dirty paper: restore the wallpaper under removed/minimized windows.
        for &(x, y, w, h) in &self.paper {
            wallpaper_rect(fb, x, y, w, h);
        }
        self.paper.clear();

        // Dirty windows, back-to-front.
        let mut order: Vec<usize> = (0..self.windows.len())
            .filter(|&i| self.windows[i].dirty && !self.windows[i].minimized)
            .collect();
        order.sort_by_key(|&i| self.windows[i].z);
        for &i in &order {
            self.render_window(fb, i);
            self.windows[i].dirty = false;
        }

        // Dirty widget: the panel (+ launcher popup when open).
        if self.panel_dirty {
            self.draw_panel(fb);
            if self.launcher_open {
                self.draw_launcher(fb);
            }
            self.panel_dirty = false;
        }
    }

    fn render_window(&self, fb: &mut Framebuffer, i: usize) {
        let w = &self.windows[i];
        let focused = i == self.focused;
        let bc = if focused { FOCUS_BORDER } else { BORDER };

        // Body + title bar + border.
        fill(fb, w.x, w.y, w.w, w.h, WIN_BG);
        fill(fb, w.x, w.y, w.w, TITLE_H, TITLE_BG);
        fill(fb, w.x, w.y, w.w, 1, bc);
        fill(fb, w.x, w.y + w.h - 1, w.w, 1, bc);
        fill(fb, w.x, w.y, 1, w.h, bc);
        fill(fb, w.x + w.w - 1, w.y, 1, w.h, bc);

        text(fb, w.x + 6, w.y + 4, &w.title, TEXT_DARK);
        // Window controls (close / maximize / minimize).
        fill(fb, w.x + w.w - 12, w.y + 5, 6, 6, CLOSE);
        fill(fb, w.x + w.w - 22, w.y + 5, 6, 6, MAXIMIZE);
        fill(fb, w.x + w.w - 32, w.y + 5, 6, 6, MINIMIZE);

        let cy = w.y + TITLE_H + 4;
        let cw = w.w.saturating_sub(12);

        match w.kind {
            Kind::Terminal => {
                fill(fb, w.x, w.y + TITLE_H, w.w, w.h - TITLE_H, TERM_BG);
                let mut ty = cy;
                for line in w.lines.iter().rev().take(6).rev() {
                    text(fb, w.x + 6, ty, line, TERM_TEXT);
                    ty += 10;
                }
                let prompt = format!("{}{}", PROMPT, w.input);
                text(fb, w.x + 6, ty, &prompt, TERM_PROMPT);
            }
            Kind::Welcome => {
                text(fb, w.x + 6, cy, "Witaj w TrangorgeOS", TEXT_DARK);
                text(fb, w.x + 6, cy + 12, "srodowisko i3dw", TEXT_MUTED);
                text(fb, w.x + 6, cy + 26, "Plasma-style floating desktop", TEXT_MUTED);
                text(fb, w.x + 6, cy + 44, "- panel z launcherem i zegarem", TEXT_DARK);
                text(fb, w.x + 6, cy + 56, "- okna plywajace z paskiem tytulu", TEXT_DARK);
                text(fb, w.x + 6, cy + 68, "- dirty cursor (mysz PS/2)", TEXT_DARK);
            }
            Kind::Monitor => {
                text(fb, w.x + 6, cy, "Monitor systemu", TEXT_DARK);
                let bars: [(&str, u32); 3] = [("Procesor", 62), ("Pamiec", 41), ("Grafika (vgpu)", 78)];
                let mut by = cy + 16;
                for (label, pct) in bars {
                    text(fb, w.x + 6, by, label, TEXT_DARK);
                    fill(fb, w.x + 6, by + 10, cw, 8, BORDER);
                    fill(fb, w.x + 7, by + 11, (cw - 2) * pct / 100, 6, ACCENT);
                    by += 22;
                }
            }
        }
    }

}

impl Plasma {
    fn draw_panel(&self, fb: &mut Framebuffer) {
        let sh = fb.height as u32;
        let sw = fb.width as u32;
        let py = sh.saturating_sub(PANEL_H);
        fill(fb, 0, py, sw, PANEL_H, PANEL_BG);
        fill(fb, 0, py, sw, 1, PANEL_EDGE);

        // Launcher button (highlighted while the menu is open).
        let btn_bg = if self.launcher_open { ACCENT } else { TASK_BG };
        fill(fb, 4, py + 4, 24, 24, btn_bg);
        for r in 0..2u32 {
            for c in 0..2u32 {
                fill(fb, 4 + 6 + c * 7, py + 4 + 6 + r * 7, 4, 4, PANEL_TEXT);
            }
        }

        // Task buttons.
        let mut tx = 36u32;
        for (i, w) in self.windows.iter().enumerate() {
            let bg = if i == self.focused { ACCENT } else { TASK_BG };
            fill(fb, tx, py + 4, 120, 24, bg);
            text(fb, tx + 6, py + 8, &w.title, PANEL_TEXT);
            tx += 124;
        }

        // Clock (right side).
        text(fb, sw - 40, py + 8, "14:32", PANEL_TEXT);
    }

    fn draw_launcher(&self, fb: &mut Framebuffer) {
        let sh = fb.height as u32;
        let panel_y = sh.saturating_sub(PANEL_H);
        let px = 8u32;
        let py = panel_y.saturating_sub(4 + 3 * 26 + 28);
        fill(fb, px, py, 220, 4 + 3 * 26 + 24, LAUNCHER_BG);
        text(fb, px + 8, py + 6, "Aplikacje", LAUNCHER_TEXT);
        text(fb, px + 8, py + 32, "Terminal", LAUNCHER_TEXT);
        text(fb, px + 8, py + 58, "Monitor systemu", LAUNCHER_TEXT);
        text(fb, px + 8, py + 84, "Witaj", LAUNCHER_TEXT);
    }
}

/// Run the interactive Plasma-like desktop loop. Returns on ESC.
pub fn run() {
    crate::terminal::set_keycode_capture(true);
    unsafe { mouse_init(); }

    let fb = match crate::gfx::console::try_fb() {
        Some(f) => f,
        None => {
            crate::terminal::set_keycode_capture(false);
            return;
        }
    };

    let (sw, sh) = (fb.width as u32, fb.height as u32);
    let mut d = Plasma::new(sw, sh);

    d.render_scene(&mut *fb);
    for w in d.windows.iter_mut() {
        w.dirty = false;
    }
    d.panel_dirty = false;
    let mut cur_buf: Vec<u32> = Vec::new();
    cur_buf.resize((CURSOR_W * CURSOR_H) as usize, 0);
    let mut cur_x = d.cursor_x;
    let mut cur_y = d.cursor_y;
    cursor_save(&*fb, cur_x, cur_y, &mut cur_buf);
    draw_cursor(&mut *fb, cur_x, cur_y);

    let mut prev_buttons = 0i32;
    let max_x = sw.saturating_sub(CURSOR_W) as i32;
    let max_y = sh.saturating_sub(CURSOR_H) as i32;

    loop {
        let mut cursor_moved = false;
        let mut full = false;

        unsafe {
            let mut dx = 0i32;
            let mut dy = 0i32;
            let mut dz = 0i32;
            let mut buttons = 0i32;
            if mouse_poll(&mut dx, &mut dy, &mut dz, &mut buttons) == 1 {
                let nx = (d.cursor_x as i32 + dx).clamp(0, max_x) as u32;
                let ny = (d.cursor_y as i32 + dy).clamp(0, max_y) as u32;
                if nx != d.cursor_x || ny != d.cursor_y {
                    d.cursor_x = nx;
                    d.cursor_y = ny;
                    cursor_moved = true;
                }
                if (buttons & 1) != 0 && (prev_buttons & 1) == 0 {
                    d.click(sh);
                    full = true;
                }
                prev_buttons = buttons;
            }
        }

        match crate::terminal::pop_keycode() {
            Some(k) => {
                if d.handle_keycode(k) {
                    break;
                }
                full = true;
            }
            None => {}
        }

        if full {
            cursor_restore(&mut *fb, cur_x, cur_y, &cur_buf);
            d.apply_dirty(&mut *fb);
            cursor_save(&*fb, d.cursor_x, d.cursor_y, &mut cur_buf);
            cur_x = d.cursor_x;
            cur_y = d.cursor_y;
            draw_cursor(&mut *fb, cur_x, cur_y);
        } else if cursor_moved {
            cursor_restore(&mut *fb, cur_x, cur_y, &cur_buf);
            cursor_save(&*fb, d.cursor_x, d.cursor_y, &mut cur_buf);
            cur_x = d.cursor_x;
            cur_y = d.cursor_y;
            draw_cursor(&mut *fb, cur_x, cur_y);
        } else {
            core::hint::spin_loop();
        }
    }

    crate::terminal::set_keycode_capture(false);
    crate::vga_buffer::WRITER.lock().clear_screen();
    crate::gfx::refresh();
}

/// C ABI hook so the linked userspace shell can launch the desktop.
#[no_mangle]
pub extern "C" fn i3dw_run() {
    run();
}


