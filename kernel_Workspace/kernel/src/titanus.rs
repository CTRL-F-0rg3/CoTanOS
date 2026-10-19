//! Interactive `titanus` desktop: a Cosmic/Hyprland-inspired environment with
//! tiling + floating windows, a vertical dock on the right, mouse + keyboard
//! input and full-redraw management. Runs in kernel space like `i3dw`/`allde`.

use alloc::string::{String, ToString};
use alloc::vec::Vec;

use crate::gfx::font::FONT8X8;
use crate::gfx::framebuffer::{Framebuffer, PixelFormat};

extern "C" {
    fn mouse_init();
    fn mouse_poll(dx: *mut i32, dy: *mut i32, dz: *mut i32, buttons: *mut i32) -> i32;
}

const DOCK_W: u32 = 56;
const TITLE_H: u32 = 20;
const CURSOR_W: u32 = 16;
const CURSOR_H: u32 = 16;
const GAP: u32 = 8;
const RADIUS: u32 = 6;

const KEY_ENTER: u32 = 0x100;
const KEY_BACKSPACE: u32 = 0x101;
const KEY_ESC: u32 = 0x102;
const KEY_RIGHT: u32 = 0x103;
const KEY_LEFT: u32 = 0x104;
const KEY_DOWN: u32 = 0x105;
const KEY_UP: u32 = 0x106;
const KEY_TAB: u32 = 0x10A;
const KEY_F2: u32 = 0x113;
const KEY_F3: u32 = 0x114;
const KEY_F5: u32 = 0x116;

// Cosmic/Hyprland palette: ciemny gradient + niebieski akcent.
const BG_TOP: u32 = 0xFF23273A;
const BG_BOTTOM: u32 = 0xFF0E0F1A;
const DOCK_BG: u32 = 0xFF181A28;
const DOCK_ENTRY: u32 = 0xFF2A2D3E;
const ACCENT: u32 = 0xFF4F8CF7;
const WIN_BG: u32 = 0xFF1C1E2C;
const TITLE_BG: u32 = 0xFF272A3A;
const BORDER: u32 = 0xFF3A3E52;
const TEXT: u32 = 0xFFE6E8F0;
const MUTED: u32 = 0xFF8A8FA0;
const PROMPT: u32 = 0xFF7CF09C;
const CLOSE: u32 = 0xFFED5A5A;

/// 16x16 arrow glyph, one row per u16, bit 15 = leftmost pixel.
const ARROW: [u16; 16] = [
    0x8000, 0xC000, 0xA000, 0x9000, 0x8800, 0x8400, 0x8200, 0x8100, //
    0x8080, 0x8040, 0x8020, 0x8010, 0xF800, 0x8000, 0x8000, 0x8000, //
];

// ── drawing helpers (flip-compensated, jak w `allde`) ────────────────────────
//
// Konsola jądra ma globalny flip framebuffera (Bochs/vgpu skanuje od dołu).
// `Framebuffer::set` sam tego nie odwraca dla Rgb888, więc kompensujemy tu,
// żeby pulpit rysował w orientacji lewy-górny = początek, bez „do góry nogami".

fn set(fb: &mut Framebuffer, x: u32, y: u32, c: u32) {
    let (w, h) = (fb.width, fb.height);
    let (x, y) = (x as usize, y as usize);
    if x >= w || y >= h {
        return;
    }
    let flip_y = fb.format == PixelFormat::Rgb888;
    let flip_x = fb.format == PixelFormat::Indexed8;
    let px = if flip_x { w - 1 - x } else { x };
    let py = if flip_y { h - 1 - y } else { y };
    fb.set(px, py, c);
}

fn get(fb: &Framebuffer, x: u32, y: u32) -> u32 {
    let (w, h) = (fb.width, fb.height);
    let (x, y) = (x as usize, y as usize);
    if x >= w || y >= h {
        return 0;
    }
    let flip_y = fb.format == PixelFormat::Rgb888;
    let flip_x = fb.format == PixelFormat::Indexed8;
    let px = if flip_x { w - 1 - x } else { x };
    let py = if flip_y { h - 1 - y } else { y };
    fb.get(px, py)
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

fn isqrt(v: u32) -> u32 {
    if v == 0 {
        return 0;
    }
    let mut g = 1u32;
    while (g + 1) * (g + 1) <= v {
        g += 1;
    }
    g
}

/// Zaokrąglony prostokąt (promień przycięty do połowy krótszego boku).
fn fill_rounded(fb: &mut Framebuffer, x: u32, y: u32, w: u32, h: u32, r: u32, c: u32) {
    if w == 0 || h == 0 {
        return;
    }
    let r = r.min(w / 2).min(h / 2);
    for row in 0..h {
        let into_corner = if row < r {
            r - row
        } else if row >= h - r {
            row - (h - r) + 1
        } else {
            0
        };
        let inset = if into_corner == 0 {
            0
        } else {
            let dy = into_corner - 1;
            r - isqrt(r * r - dy * dy)
        };
        fill(fb, x + inset, y + row, w - 2 * inset, 1, c);
    }
}

/// Styl tapety. Kernel nie ładuje PNG, więc tapety są proceduralne (generowane).
#[derive(Clone, Copy, PartialEq, Eq)]
enum Wallpaper {
    Cosmic, // pionowy gradient (ciemnoniebieski)
    Aurora, // ukośny gradient niebieski → fioletowy
    Radial, // poświata ze środka
    Ripple, // koncentryczne pierścienie
    Grid,   // subtelna siatka
    Waves,  // fale
}

impl Wallpaper {
    const ALL: [Wallpaper; 6] = [
        Wallpaper::Cosmic,
        Wallpaper::Aurora,
        Wallpaper::Radial,
        Wallpaper::Ripple,
        Wallpaper::Grid,
        Wallpaper::Waves,
    ];

    fn next(self) -> Wallpaper {
        let idx = Self::ALL.iter().position(|&w| w == self).unwrap_or(0);
        Self::ALL[(idx + 1) % Self::ALL.len()]
    }

    fn name(self) -> &'static str {
        match self {
            Wallpaper::Cosmic => "Cosmic",
            Wallpaper::Aurora => "Aurora",
            Wallpaper::Radial => "Radial",
            Wallpaper::Ripple => "Ripple",
            Wallpaper::Grid => "Grid",
            Wallpaper::Waves => "Waves",
        }
    }
}

/// Miesza dwa kolory 0xRRGGBB w proporcji `t` (0..=255).
fn lerp_color(a: u32, b: u32, t: u32) -> u32 {
    let t = t.min(255);
    let inv = 255 - t;
    let r = (((a >> 16) & 0xFF) * inv + ((b >> 16) & 0xFF) * t) / 255;
    let g = (((a >> 8) & 0xFF) * inv + ((b >> 8) & 0xFF) * t) / 255;
    let bl = ((a & 0xFF) * inv + (b & 0xFF) * t) / 255;
    0xFF00_0000 | (r << 16) | (g << 8) | bl
}

/// Kolor piksela tapety dla danego stylu (czysta arytmetyka całkowita).
fn wallpaper_pixel(style: Wallpaper, x: u32, y: u32, w: u32, h: u32) -> u32 {
    match style {
        Wallpaper::Cosmic => {
            let t = if h <= 1 { 0 } else { y * 255 / (h - 1) };
            lerp_color(BG_TOP, BG_BOTTOM, t)
        }
        Wallpaper::Aurora => {
            let denom = (w + h).saturating_sub(2).max(1);
            let t = (x + y) * 255 / denom;
            lerp_color(0xFF23273A, 0xFF3A1E4E, t)
        }
        Wallpaper::Radial => {
            let dx = x as i32 - (w / 2) as i32;
            let dy = y as i32 - (h / 2) as i32;
            let dist = isqrt((dx * dx + dy * dy) as u32);
            let maxd = isqrt(((w / 2) * (w / 2) + (h / 2) * (h / 2)) as u32).max(1);
            let t = (dist * 255 / maxd).min(255);
            lerp_color(0xFF2A4A7A, 0xFF0A0C16, t)
        }
        Wallpaper::Ripple => {
            let dx = x as i32 - (w / 2) as i32;
            let dy = y as i32 - (h / 2) as i32;
            let dist = isqrt((dx * dx + dy * dy) as u32);
            if (dist / 48) % 2 == 0 { 0xFF1C2233 } else { 0xFF12151F }
        }
        Wallpaper::Grid => {
            if x % 40 == 0 || y % 40 == 0 { 0xFF141826 } else { 0xFF10131E }
        }
        Wallpaper::Waves => {
            match ((x + y) / 28) % 5 {
                0 => 0xFF16203A,
                1 => 0xFF101828,
                2 => 0xFF1A2438,
                3 => 0xFF0E1420,
                _ => 0xFF121A2C,
            }
        }
    }
}

/// Pełna tapeta (dla danego stylu).
fn wallpaper(fb: &mut Framebuffer, style: Wallpaper) {
    let w = fb.width as u32;
    let h = fb.height as u32;
    for y in 0..h {
        for x in 0..w {
            set(fb, x, y, wallpaper_pixel(style, x, y, w, h));
        }
    }
}

/// Przywraca tapetę tylko w jednym prostokącie (dirty wallpaper) — nie całą.
fn wallpaper_rect(fb: &mut Framebuffer, x: u32, y: u32, w: u32, h: u32, style: Wallpaper) {
    let fw = fb.width as u32;
    let fh = fb.height as u32;
    for row in 0..h {
        let gy = y + row;
        if gy >= fh {
            break;
        }
        for col in 0..w {
            let gx = x + col;
            if gx >= fw {
                break;
            }
            set(fb, gx, gy, wallpaper_pixel(style, gx, gy, fw, fh));
        }
    }
}

/// Czy dwa prostokąty się nakładają.
fn rects_overlap(a: (u32, u32, u32, u32), b: (u32, u32, u32, u32)) -> bool {
    let (ax, ay, aw, ah) = a;
    let (bx, by, bw, bh) = b;
    ax < bx + bw && bx < ax + aw && ay < by + bh && by < ay + ah
}


// ── cursor (dirty: zapamiętaj/przywróć 16x16 pod kursorem) ───────────────────

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

fn draw_cursor(fb: &mut Framebuffer, x: u32, y: u32) {
    // Bez obrysu przesuniętego o ±1 px: wychodziłby poza 16×16 obszar
    // zapisu/przywrócenia i zostawiał artefakty. Biała strzałka jest widoczna
    // na ciemnym tle.
    draw_shape(fb, x as i32, y as i32, 0xFFFFFFFF);
}

fn cursor_save(fb: &Framebuffer, x: u32, y: u32, buf: &mut [u32; 256]) {
    for row in 0..CURSOR_H {
        for col in 0..CURSOR_W {
            buf[(row * CURSOR_W + col) as usize] = get(fb, x + col, y + row);
        }
    }
}

fn cursor_restore(fb: &mut Framebuffer, x: u32, y: u32, buf: &[u32; 256]) {
    for row in 0..CURSOR_H {
        for col in 0..CURSOR_W {
            set(fb, x + col, y + row, buf[(row * CURSOR_W + col) as usize]);
        }
    }
}

// ── okna i pulpit ────────────────────────────────────────────────────────────

#[derive(Clone, Copy, PartialEq, Eq)]
enum Kind {
    Terminal,
    Monitor,
    Welcome,
}

struct Window {
    kind: Kind,
    title: String,
    /// Geometria pływającego okna; dla kafelkowego ignorowana.
    x: u32,
    y: u32,
    w: u32,
    h: u32,
    floating: bool,
    z: u32,
    minimized: bool,
    input: String,
    /// Historia komend/wyników terminala (linie wyświetlane w oknie).
    lines: Vec<String>,
    /// Czy okno wymaga przerysowania (dirty window).
    dirty: bool,
}

struct Titanus {
    windows: Vec<Window>,
    focused: usize,
    cursor_x: u32,
    cursor_y: u32,
    next_z: u32,
    screen_w: u32,
    screen_h: u32,
    dragging: Option<usize>,
    /// Prostokąty, w których trzeba przywrócić tapetę (dirty wallpaper).
    wallpaper_dirty: Vec<(u32, u32, u32, u32)>,
    /// Czy dok wymaga przerysowania (dirty widget).
    dock_dirty: bool,
    /// Aktualny styl tapety (zarządzanie tapetami).
    wallpaper: Wallpaper,
    /// Pełne przerysowanie (po zmianie tapety / starcie).
    full_redraw: bool,
}

impl Titanus {
    fn new(sw: u32, sh: u32) -> Self {
        let mut t = Self {
            windows: Vec::new(),
            focused: usize::MAX,
            cursor_x: sw / 2,
            cursor_y: sh / 2,
            next_z: 1,
            screen_w: sw,
            screen_h: sh,
            dragging: None,
            wallpaper_dirty: Vec::new(),
            dock_dirty: true,
            wallpaper: Wallpaper::Cosmic,
            full_redraw: true,
        };
        // Domyślny układ: terminal kafelkowy + okno powitalne pływające.
        t.add_window(Kind::Terminal, false);
        t.add_window(Kind::Welcome, true);
        t
    }

    fn add_window(&mut self, kind: Kind, floating: bool) {
        let (title, w, h) = match kind {
            Kind::Terminal => ("Terminal", 480u32, 300u32),
            Kind::Monitor => ("Monitor", 380u32, 240u32),
            Kind::Welcome => ("Witaj", 400u32, 260u32),
        };
        let n = self.windows.len() as u32;
        let x = 80 + (n % 4) * 28;
        let y = 60 + (n % 4) * 28;
        self.windows.push(Window {
            kind,
            title: title.to_string(),
            x,
            y,
            w,
            h,
            floating,
            z: self.next_z,
            minimized: false,
            input: String::new(),
            lines: Vec::new(),
            dirty: true,
        });
        self.next_z += 1;
        self.focused = self.windows.len() - 1;
        if !floating {
            // Kafelkowanie się przeliczyło: przywróć tapetę w obszarze roboczym.
            let (wx, wy, ww, wh) = self.work_area();
            self.wallpaper_dirty.push((wx, wy, ww, wh));
            for w in self.windows.iter_mut() {
                if !w.floating {
                    w.dirty = true;
                }
            }
        }
        self.dock_dirty = true;
    }

    fn close_focused(&mut self) {
        if self.focused >= self.windows.len() {
            return;
        }
        let was_floating = self.windows[self.focused].floating;
        // Przywróć tapetę pod zamykanym oknem.
        if let Some(r) = self.rect_of(self.focused) {
            self.wallpaper_dirty.push(r);
        }
        self.windows.remove(self.focused);
        self.focused = if self.windows.is_empty() {
            usize::MAX
        } else {
            self.focused.min(self.windows.len() - 1)
        };
        if !was_floating {
            // Kafelkowanie się przeliczyło: cały obszar roboczy + kafelki.
            let (wx, wy, ww, wh) = self.work_area();
            self.wallpaper_dirty.push((wx, wy, ww, wh));
            for w in self.windows.iter_mut() {
                if !w.floating {
                    w.dirty = true;
                }
            }
        }
        self.dock_dirty = true;
    }

    fn focus_to(&mut self, idx: usize) {
        if idx >= self.windows.len() {
            return;
        }
        let old = self.focused;
        self.focused = idx;
        self.next_z += 1;
        self.windows[idx].z = self.next_z;
        if old < self.windows.len() {
            self.windows[old].dirty = true;
        }
        self.windows[idx].dirty = true;
        self.dock_dirty = true;
    }

    fn focus_next(&mut self) {
        if self.windows.is_empty() {
            return;
        }
        let n = self.windows.len();
        let next = if self.focused >= n { 0 } else { (self.focused + 1) % n };
        self.focus_to(next);
    }

    /// Przełącza na następną tapetę (zarządzanie tapetami).
    fn cycle_wallpaper(&mut self) {
        self.wallpaper = self.wallpaper.next();
        self.full_redraw = true;
        self.dock_dirty = true;
    }

    /// Obszar roboczy: ekran minus pionowy dok po prawej.
    fn work_area(&self) -> (u32, u32, u32, u32) {
        (0, 0, self.screen_w.saturating_sub(DOCK_W), self.screen_h)
    }

    /// Prostokąt okna w tej chwili (kafelki liczone, pływające ze stanu).
    fn rect_of(&self, idx: usize) -> Option<(u32, u32, u32, u32)> {
        let w = &self.windows[idx];
        if w.minimized {
            return None;
        }
        if w.floating {
            return Some((w.x, w.y, w.w, w.h));
        }
        let (wx, wy, ww, wh) = self.work_area();
        let tiled: Vec<usize> = self
            .windows
            .iter()
            .enumerate()
            .filter(|(_, x)| !x.floating && !x.minimized)
            .map(|(i, _)| i)
            .collect();
        let n = tiled.len().max(1) as u32;
        let total_gap = GAP * n.saturating_sub(1);
        let cw = if ww > total_gap { (ww - total_gap) / n } else { ww };
        let col = tiled.iter().position(|&j| j == idx)?;
        Some((wx + (col as u32) * (cw + GAP), wy, cw, wh))
    }

    fn window_at(&self, x: u32, y: u32) -> Option<usize> {
        let mut best: Option<usize> = None;
        let mut best_z = 0u32;
        for i in 0..self.windows.len() {
            if let Some((rx, ry, rw, rh)) = self.rect_of(i) {
                if x >= rx && x < rx + rw && y >= ry && y < ry + rh {
                    if best.is_none() || self.windows[i].z > best_z {
                        best = Some(i);
                        best_z = self.windows[i].z;
                    }
                }
            }
        }
        best
    }

    fn dock_click(&mut self, x: u32, y: u32) {
        let dx = self.screen_w - DOCK_W;
        let side = DOCK_W - 12;
        let sx = dx + 6;
        // Kafelki aplikacji (T=terminal tiled, M/W = pływające).
        let apps = [(Kind::Terminal, false), (Kind::Monitor, true), (Kind::Welcome, true)];
        for (i, (kind, floating)) in apps.iter().enumerate() {
            let ty = 12 + i as u32 * (side + 10);
            if x >= sx && x < sx + side && y >= ty && y < ty + side {
                self.add_window(*kind, *floating);
                return;
            }
        }
        // Lista zadań (fokus).
        let mut task_y = 12 + apps.len() as u32 * (side + 10) + 10;
        for i in 0..self.windows.len() {
            if x >= sx && x < sx + side && y >= task_y && y < task_y + side {
                self.focus_to(i);
                return;
            }
            task_y += side + 6;
        }
        // Przycisk tapety (na dole doku) — cykliczna zmiana stylu.
        let wp_y = self.screen_h.saturating_sub(48);
        if x >= sx && x < sx + side && y >= wp_y && y < wp_y + side {
            self.cycle_wallpaper();
        }
    }

    fn handle_click(&mut self) {
        let (x, y) = (self.cursor_x, self.cursor_y);
        if x >= self.screen_w.saturating_sub(DOCK_W) {
            self.dock_click(x, y);
            return;
        }
        let Some(i) = self.window_at(x, y) else { return };
        let Some((rx, ry, rw, rh)) = self.rect_of(i) else { return };
        // Przycisk zamknięcia (x w prawym górnym rogu paska tytułu).
        if x >= rx + rw - 16 && x < rx + rw - 2 && y >= ry + 2 && y < ry + TITLE_H {
            let was_floating = self.windows[i].floating;
            self.wallpaper_dirty.push((rx, ry, rw, rh));
            self.windows.remove(i);
            self.focused = if self.windows.is_empty() {
                usize::MAX
            } else {
                self.focused.min(self.windows.len() - 1)
            };
            if !was_floating {
                let (wx, wy, ww, wh) = self.work_area();
                self.wallpaper_dirty.push((wx, wy, ww, wh));
                for w in self.windows.iter_mut() {
                    if !w.floating {
                        w.dirty = true;
                    }
                }
            }
            self.dock_dirty = true;
            return;
        }
        self.focus_to(i);
        // Przeciąganie: złap pływające okno za pasek tytułu.
        if self.windows[i].floating && y < ry + TITLE_H {
            self.dragging = Some(i);
        }
    }

    fn handle_drag(&mut self, dx: i32, dy: i32) {
        let Some(i) = self.dragging else { return };
        if !self.windows[i].floating {
            return;
        }
        // Stary prostokąt okna → przywróć tam tapetę.
        self.wallpaper_dirty
            .push((self.windows[i].x, self.windows[i].y, self.windows[i].w, self.windows[i].h));
        let max_x = self.screen_w.saturating_sub(self.windows[i].w) as i64;
        let max_y = self.screen_h.saturating_sub(self.windows[i].h) as i64;
        let nx = (self.windows[i].x as i64 + dx as i64).clamp(0, max_x) as u32;
        let ny = (self.windows[i].y as i64 + dy as i64).clamp(0, max_y) as u32;
        self.windows[i].x = nx;
        self.windows[i].y = ny;
        self.windows[i].dirty = true;
    }

    fn type_char(&mut self, c: char) {
        if self.focused >= self.windows.len() {
            return;
        }
        let focused = self.focused;
        if self.windows[focused].kind != Kind::Terminal {
            // Poza oknem terminala znaki ignorujemy.
            return;
        }
        match c {
            '\x08' => {
                self.windows[focused].input.pop();
                self.windows[focused].dirty = true;
            }
            '\n' => {
                let cmd = core::mem::take(&mut self.windows[focused].input);
                self.run_terminal_command(focused, cmd);
            }
            c if (c as u32) >= 0x20 && (c as u32) < 0x7F => {
                self.windows[focused].input.push(c);
                self.windows[focused].dirty = true;
            }
            _ => {}
        }
    }

    /// Wykonuje komendę wpisaną w oknie terminala i dopisuje wynik do historii.
    fn run_terminal_command(&mut self, idx: usize, cmd: String) {
        if idx >= self.windows.len() || self.windows[idx].kind != Kind::Terminal {
            return;
        }
        self.windows[idx]
            .lines
            .push(format!("user@titanus:~$ {}", cmd));
        let t = cmd.trim();
        match t {
            "" => {}
            "help" => {
                self.windows[idx].lines.push("help   - ta lista".to_string());
                self.windows[idx].lines.push("echo X - wypisz tekst X".to_string());
                self.windows[idx].lines.push("clear  - wyczysc ekran".to_string());
                self.windows[idx].lines.push("uname  - wersja systemu".to_string());
                self.windows[idx].lines.push("list   - lista okien".to_string());
                self.windows[idx].lines.push("new    - nowy terminal".to_string());
                self.windows[idx].lines.push("wall   - zmien tapete".to_string());
                self.windows[idx].lines.push("exit   - zamknij to okno".to_string());
            }
            "clear" => {
                self.windows[idx].lines.clear();
            }
            "uname" => {
                self.windows[idx]
                    .lines
                    .push("TrangorgeOS (Titanus DE, kafelki + plywajace)".to_string());
            }
            "list" => {
                let list: Vec<String> = self
                    .windows
                    .iter()
                    .enumerate()
                    .map(|(i, w)| format!("[{}] {} z={}", i, w.title, w.z))
                    .collect();
                self.windows[idx].lines.extend(list);
            }
            "new" => {
                self.add_window(Kind::Terminal, false);
                self.windows[idx].lines.push("otwarto nowy terminal".to_string());
            }
            "wall" => {
                self.cycle_wallpaper();
                self.windows[idx].lines.push("zmieniono tapete".to_string());
            }
            "exit" => {
                self.close_focused();
                return;
            }
            _ if t.starts_with("echo ") => {
                self.windows[idx].lines.push(t[5..].to_string());
            }
            _ => {
                self.windows[idx]
                    .lines
                    .push(format!("nieznana komenda: {}", t));
            }
        }
        // Przytnij historię, żeby zmieściła się w oknie.
        while self.windows[idx].lines.len() > 40 {
            self.windows[idx].lines.remove(0);
        }
        self.windows[idx].dirty = true;
        self.dock_dirty = true;
    }

    fn handle_keycode(&mut self, k: u32) -> bool {
        match k {
            KEY_ESC => return true,
            KEY_TAB => self.focus_next(),
            KEY_F2 => self.close_focused(),
            KEY_F3 => self.add_window(Kind::Terminal, false),
            KEY_F5 => self.cycle_wallpaper(),
            KEY_RIGHT => self.cursor_x = self.cursor_x.saturating_add(20).min(self.screen_w.saturating_sub(1)),
            KEY_LEFT => self.cursor_x = self.cursor_x.saturating_sub(20),
            KEY_UP => self.cursor_y = self.cursor_y.saturating_sub(20),
            KEY_DOWN => self.cursor_y = self.cursor_y.saturating_add(20).min(self.screen_h.saturating_sub(1)),
            KEY_BACKSPACE => self.type_char('\x08'),
            KEY_ENTER => self.type_char('\n'),
            _ => {}
        }
        false
    }
}

impl Titanus {
    fn draw_window(&self, fb: &mut Framebuffer, idx: usize, x: u32, y: u32, w: u32, h: u32) {
        let win = &self.windows[idx];
        let border = if idx == self.focused { ACCENT } else { BORDER };
        fill_rounded(fb, x, y, w, h, RADIUS, border);
        fill_rounded(fb, x + 1, y + 1, w - 2, h - 2, RADIUS - 1, WIN_BG);
        fill(fb, x + 1, y + 1, w - 2, TITLE_H, TITLE_BG);
        text(fb, x + 6, y + 4, &win.title, TEXT);
        text(fb, x + w - 16, y + 4, "x", CLOSE);

        let cy = y + TITLE_H + 8;
        match win.kind {
            Kind::Terminal => {
                // Historia komend (ostatnie wiersze mieszczące się w oknie),
                // potem prompt + bieżące wejście.
                let content_h = h.saturating_sub(TITLE_H + 8 + 8);
                let max_lines = (content_h / 13).max(2) as usize;
                let skip = win.lines.len().saturating_sub(max_lines.saturating_sub(1));
                let mut ly = cy;
                for line in win.lines.iter().skip(skip) {
                    text(fb, x + 6, ly, line, TEXT);
                    ly += 13;
                }
                text(fb, x + 6, ly, "user@titanus:~$", PROMPT);
                text(fb, x + 6 + 8 * 14, ly, &win.input, TEXT);
            }
            Kind::Monitor => {
                let cw = w.saturating_sub(14);
                for (i, (label, pct)) in
                    [("Procesor", 62u32), ("Pamiec", 41u32), ("vgpu", 78u32)].iter().enumerate()
                {
                    let by = cy + i as u32 * 22;
                    text(fb, x + 6, by, label, TEXT);
                    fill(fb, x + 6, by + 12, cw, 8, BORDER);
                    fill(fb, x + 7, by + 13, (cw - 2) * pct / 100, 6, ACCENT);
                }
            }
            Kind::Welcome => {
                text(fb, x + 6, cy, "Witaj w Titanus DE", TEXT);
                text(fb, x + 6, cy + 12, "kafelki + plywajace", MUTED);
                text(fb, x + 6, cy + 26, "dok po prawej", MUTED);
                text(fb, x + 6, cy + 40, "mysz: klik=fokus/przeciaganie", MUTED);
            }
        }
    }

    fn draw_dock(&self, fb: &mut Framebuffer) {
        let dx = self.screen_w - DOCK_W;
        fill(fb, dx, 0, DOCK_W, self.screen_h, DOCK_BG);
        let side = DOCK_W - 12;
        let sx = dx + 6;
        let apps = ["T", "M", "W"];
        for (i, label) in apps.iter().enumerate() {
            let ty = 12 + i as u32 * (side + 10);
            fill_rounded(fb, sx, ty, side, side, 8, DOCK_ENTRY);
            text(fb, sx + side / 2 - 3, ty + side / 2 - 4, label, TEXT);
        }
        let mut task_y = 12 + apps.len() as u32 * (side + 10) + 10;
        for (i, w) in self.windows.iter().enumerate() {
            let bg = if i == self.focused { ACCENT } else { DOCK_ENTRY };
            fill_rounded(fb, sx, task_y, side, side, 8, bg);
            let label: String = w.title.chars().next().map(|c| c.to_string()).unwrap_or_default();
            text(fb, sx + side / 2 - 3, task_y + side / 2 - 4, &label, TEXT);
            task_y += side + 6;
        }
        // Przycisk tapety (na dole doku) — pierwsza litera nazwy stylu.
        let wp_y = self.screen_h.saturating_sub(48);
        fill_rounded(fb, sx, wp_y, side, side, 8, DOCK_ENTRY);
        let wpl: String = self.wallpaper.name().chars().next().map(|c| c.to_string()).unwrap_or_default();
        text(fb, sx + side / 2 - 3, wp_y + side / 2 - 4, &wpl, TEXT);
        text(fb, dx + 8, self.screen_h.saturating_sub(16), "14:32", TEXT);
    }

    fn render_scene(&self, fb: &mut Framebuffer) {
        wallpaper(fb, self.wallpaper);
        let (wx, wy, ww, wh) = self.work_area();
        let tiled: Vec<usize> = self
            .windows
            .iter()
            .enumerate()
            .filter(|(_, w)| !w.floating && !w.minimized)
            .map(|(i, _)| i)
            .collect();
        let n = tiled.len().max(1) as u32;
        let total_gap = GAP * n.saturating_sub(1);
        let cw = if ww > total_gap { (ww - total_gap) / n } else { ww };
        for (col, &i) in tiled.iter().enumerate() {
            let cx = wx + (col as u32) * (cw + GAP);
            self.draw_window(fb, i, cx, wy, cw, wh);
        }
        for (i, w) in self.windows.iter().enumerate() {
            if w.floating && !w.minimized {
                self.draw_window(fb, i, w.x, w.y, w.w, w.h);
            }
        }
        self.draw_dock(fb);
    }

    fn needs_redraw(&self) -> bool {
        !self.wallpaper_dirty.is_empty()
            || self.dock_dirty
            || self.windows.iter().any(|w| w.dirty)
    }

    fn clear_dirty(&mut self) {
        self.wallpaper_dirty.clear();
        self.dock_dirty = false;
        for w in self.windows.iter_mut() {
            w.dirty = false;
        }
    }

    /// Selektywne odświeżenie: tylko brudne regiony, nie cała scena.
    ///
    /// Kolejność warstw (z-order) jest tu jawnie egzekwowana:
    ///   0. tapeta (tylko brudne prostokąty),
    ///   1. okna (kafelki w kolumnach, pływające wg `z`),
    ///   2. dok (pionowy, po prawej),
    ///   3. kursor (rysowany poza tą metodą, na samym wierzchu).
    fn apply(&mut self, fb: &mut Framebuffer) {
        // 1. Tapeta tylko w brudnych prostokątach (dirty wallpaper).
        let wd = core::mem::take(&mut self.wallpaper_dirty);
        for &(x, y, w, h) in &wd {
            wallpaper_rect(fb, x, y, w, h, self.wallpaper);
        }

        // 2. Zbierz okna do przerysowania i namaluj je w kolejności z-order:
        //    - okna brudne,
        //    - okna leżące NAD brudnym oknem, które się z nim nakładają,
        //    - okna nakładające się na przywróconą tapetę (inaczej przywrócenie
        //      tapety "zjadłoby" pływające okna i zostawiło jaśniejszy ślad).
        let mut to_redraw: Vec<usize> = Vec::new();
        for (i, w) in self.windows.iter().enumerate() {
            if w.minimized {
                continue;
            }
            let Some((wx, wy, ww, wh)) = self.rect_of(i) else { continue };
            let mut mark = w.dirty;
            if !mark {
                for (j, other) in self.windows.iter().enumerate() {
                    if j == i || other.minimized || !other.dirty {
                        continue;
                    }
                    if other.z < self.windows[i].z {
                        if let Some((ox, oy, ow, oh)) = self.rect_of(j) {
                            if rects_overlap((wx, wy, ww, wh), (ox, oy, ow, oh)) {
                                mark = true;
                                break;
                            }
                        }
                    }
                }
            }
            if !mark {
                for &(dx, dy, dw, dh) in &wd {
                    if rects_overlap((wx, wy, ww, wh), (dx, dy, dw, dh)) {
                        mark = true;
                        break;
                    }
                }
            }
            if mark {
                to_redraw.push(i);
            }
        }
        to_redraw.sort_by_key(|&i| self.windows[i].z);
        to_redraw.dedup();
        for i in to_redraw {
            if let Some((x, y, w, h)) = self.rect_of(i) {
                self.draw_window(fb, i, x, y, w, h);
            }
        }
        for w in self.windows.iter_mut() {
            w.dirty = false;
        }

        // 3. Dok (dirty widget), nad oknami.
        if self.dock_dirty {
            self.draw_dock(fb);
            self.dock_dirty = false;
        }
    }
}

/// Run the interactive Titanus desktop loop. Returns on ESC.
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
    let mut d = Titanus::new(sw, sh);
    let mut cur_buf = [0u32; 256];
    let (mut cx, mut cy) = (d.cursor_x, d.cursor_y);

    // Scena + kursor (dirty: zapamiętaj tło pod kursorem).
    d.render_scene(&mut *fb);
    d.clear_dirty();
    cursor_save(&*fb, cx, cy, &mut cur_buf);
    draw_cursor(&mut *fb, cx, cy);

    let mut prev_buttons = 0i32;

    loop {
        let (mut dx, mut dy, mut dz, mut btn) = (0i32, 0i32, 0i32, 0i32);
        unsafe {
            mouse_poll(&mut dx, &mut dy, &mut dz, &mut btn);
        }

        let moved = dx != 0 || dy != 0;
        // PS/2 zgłasza Y odwrócone (ruch w górę = dodatnie dy); negujemy raz
        // i używamy tej samej wartości dla kursora i przeciągania okna.
        let ndy = -dy;

        // Zdejmij kursor, zanim zmieni się scena.
        if moved {
            cursor_restore(&mut *fb, cx, cy, &cur_buf);
        }

        // Przeciąganie pływającego okna.
        if d.dragging.is_some() && moved {
            d.handle_drag(dx, ndy);
        }
        if moved {
            d.cursor_x = (d.cursor_x as i32 + dx).max(0).min(sw as i32 - 1) as u32;
            d.cursor_y = (d.cursor_y as i32 + ndy).max(0).min(sh as i32 - 1) as u32;
            cx = d.cursor_x;
            cy = d.cursor_y;
        }

        // Klik (lewy przycisk).
        let pressed = (btn & 1) != 0 && (prev_buttons & 1) == 0;
        let released = (btn & 1) == 0 && (prev_buttons & 1) != 0;
        if pressed {
            d.handle_click();
        }
        if released {
            d.dragging = None;
        }
        prev_buttons = btn;

        // Klawiatura.
        let mut quit = false;
        while let Some(k) = crate::terminal::pop_keycode() {
            if d.handle_keycode(k) {
                quit = true;
            }
        }

        // Pełne odświeżenie (zmiana tapety) albo selektywne (brudne regiony).
        let redrew = d.full_redraw || d.needs_redraw();
        if d.full_redraw {
            d.render_scene(&mut *fb);
            d.clear_dirty();
            d.full_redraw = false;
        } else if d.needs_redraw() {
            d.apply(&mut *fb);
        }

        // Przywróć kursor po ruchu albo po pełnym odświeżeniu.
        if moved || redrew {
            cursor_save(&*fb, cx, cy, &mut cur_buf);
            draw_cursor(&mut *fb, cx, cy);
        }

        if quit {
            break;
        }
        if !moved && btn == 0 {
            core::hint::spin_loop();
        }
    }

    crate::terminal::set_keycode_capture(false);
    crate::vga_buffer::WRITER.lock().clear_screen();
    crate::gfx::refresh();
}

/// C ABI hook: the linked userspace shell launches the Titanus desktop.
#[no_mangle]
pub extern "C" fn titanus_run() {
    run();
}




