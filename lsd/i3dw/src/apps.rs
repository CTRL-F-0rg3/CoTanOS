//! Applications the desktop can spawn, and how their windows look.
//!
//! Each app is an [`AppKind`] with a title, a launcher label, a default window
//! size, and a content renderer. The content renderer draws into the window's
//! content area (below the title bar) in screen coordinates.

use uspace::gfx::render::{draw_rect, fill_rect, Color, rgb};
use uspace::gfx::text::{self, Align};

use crate::theme;

/// The application kinds the launcher offers.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum AppKind {
    Welcome,
    Terminal,
    FileManager,
    SystemMonitor,
    Settings,
}

impl AppKind {
    pub fn title(self) -> &'static str {
        match self {
            AppKind::Welcome => "Witaj",
            AppKind::Terminal => "Terminal",
            AppKind::FileManager => "Pliki",
            AppKind::SystemMonitor => "Monitor systemu",
            AppKind::Settings => "Ustawienia",
        }
    }

    /// Short label for the launcher list.
    pub fn label(self) -> &'static str {
        match self {
            AppKind::Welcome => "Witaj",
            AppKind::Terminal => "Terminal",
            AppKind::FileManager => "Pliki",
            AppKind::SystemMonitor => "Monitor",
            AppKind::Settings => "Ustawienia",
        }
    }

    /// The default floating window size for this app.
    pub fn default_size(self) -> (u32, u32) {
        match self {
            AppKind::Welcome => (480, 300),
            AppKind::Terminal => (520, 340),
            AppKind::FileManager => (400, 400),
            AppKind::SystemMonitor => (420, 320),
            AppKind::Settings => (440, 360),
        }
    }

    pub fn all() -> [AppKind; 5] {
        [
            AppKind::Welcome,
            AppKind::Terminal,
            AppKind::FileManager,
            AppKind::SystemMonitor,
            AppKind::Settings,
        ]
    }
}

/// Draw the content of an app into its window's content area.
///
/// `(x, y)` is the top-left of the content area, `(w, h)` its size. Drawing is
/// done in screen coordinates; `buf` is the whole framebuffer.
pub fn render_content(
    kind: AppKind,
    buf: &mut [u32],
    sw: u32,
    sh: u32,
    x: i32,
    y: i32,
    w: u32,
    h: u32,
) {
    match kind {
        AppKind::Welcome => draw_welcome(buf, sw, sh, x, y, w, h),
        AppKind::Terminal => draw_terminal(buf, sw, sh, x, y, w, h),
        AppKind::FileManager => draw_file_manager(buf, sw, sh, x, y, w, h),
        AppKind::SystemMonitor => draw_system_monitor(buf, sw, sh, x, y, w, h),
        AppKind::Settings => draw_settings(buf, sw, sh, x, y, w, h),
    }
}

const PAD: i32 = 12;
const LEADING: i32 = 22;

/// Draw one line of text, clipped to the content box width.
fn line(
    buf: &mut [u32],
    sw: u32,
    sh: u32,
    x: i32,
    y: i32,
    box_w: u32,
    size: u32,
    s: &str,
    c: Color,
) {
    text::draw_clipped(buf, sw, sh, x, y, size, s, c, box_w, Align::Left);
}

// ── welcome ──────────────────────────────────────────────────────────────────

fn draw_welcome(buf: &mut [u32], sw: u32, sh: u32, x: i32, y: i32, w: u32, h: u32) {
    let _ = h;
    let mut ty = y + PAD;
    line(buf, sw, sh, x + PAD, ty, w - 2 * PAD as u32, 19, "Witaj w TrangorgeOS", theme::TEXT_DARK);
    ty += 26;
    line(buf, sw, sh, x + PAD, ty, w - 2 * PAD as u32, 14, "Środowisko i3dw — pulpit w stylu KDE Plasma", theme::TEXT_MUTED);
    ty += LEADING + 4;
    fill_rect(buf, sw, sh, x + PAD, ty, w - 2 * PAD as u32, 1, theme::BORDER);
    ty += 10;

    for text in [
        "• Okna pływające z paskiem tytułu",
        "• Panel z launcherem i zegarem",
        "• Aplikacje: Terminal, Pliki, Monitor, Ustawienia",
        "• Uruchamiane komendą i3dw",
    ] {
        line(buf, sw, sh, x + PAD, ty, w - 2 * PAD as u32, 14, text, theme::TEXT_DARK);
        ty += LEADING;
    }
}

// ── terminal ─────────────────────────────────────────────────────────────────

const TERM_BG: Color = rgb(0x1E, 0x1E, 0x28);
const TERM_PROMPT: Color = rgb(0x7C, 0xF0, 0x9C);
const TERM_TEXT: Color = rgb(0xD0, 0xD8, 0xE0);

fn draw_terminal(buf: &mut [u32], sw: u32, sh: u32, x: i32, y: i32, w: u32, h: u32) {
    fill_rect(buf, sw, sh, x, y, w, h, TERM_BG);

    let mut ty = y + PAD;
    let entries: &[(&str, Color)] = &[
        ("user@i3dw:~$ i3dw", TERM_PROMPT),
        ("uruchamiam środowisko graficzne…", TERM_TEXT),
        ("user@i3dw:~$ uname", TERM_PROMPT),
        ("TrangorgeOS", TERM_TEXT),
        ("user@i3dw:~$ ls", TERM_PROMPT),
        ("Dokumenty  Obrazy  Muzyka  notatka.txt", TERM_TEXT),
        ("user@i3dw:~$ █", TERM_PROMPT),
    ];
    for (s, c) in entries {
        line(buf, sw, sh, x + PAD, ty, w - 2 * PAD as u32, 14, s, *c);
        ty += LEADING;
    }
}

// ── file manager ─────────────────────────────────────────────────────────────

fn draw_file_manager(buf: &mut [u32], sw: u32, sh: u32, x: i32, y: i32, w: u32, h: u32) {
    let _ = h;
    let mut ty = y + PAD;
    line(buf, sw, sh, x + PAD, ty, w - 2 * PAD as u32, 14, "Pliki — /home/user", theme::TEXT_MUTED);
    ty += LEADING + 4;

    // (name, is_dir, icon colour)
    let entries: &[(&str, bool, Color)] = &[
        ("Dokumenty", true, rgb(0x3D, 0xAE, 0xE9)),
        ("Obrazy", true, rgb(0x2E, 0xCC, 0x71)),
        ("Muzyka", true, rgb(0x9B, 0x59, 0xB6)),
        ("Pobrane", true, rgb(0xF5, 0xC2, 0x11)),
        ("notatka.txt", false, theme::TEXT_MUTED),
        ("projekt.rs", false, theme::ACCENT),
    ];
    for (name, is_dir, icon) in entries {
        fill_rect(buf, sw, sh, x + PAD, ty + 3, 12, 12, *icon);
        let tag = if *is_dir { "D" } else { "F" };
        line(buf, sw, sh, x + PAD + 18, ty, 12, 12, tag, theme::TEXT_MUTED);
        line(buf, sw, sh, x + PAD + 34, ty, w - PAD as u32 - 34, 14, name, theme::TEXT_DARK);
        ty += LEADING + 2;
    }
}

// ── system monitor ───────────────────────────────────────────────────────────

fn draw_system_monitor(buf: &mut [u32], sw: u32, sh: u32, x: i32, y: i32, w: u32, h: u32) {
    let _ = h;
    let mut ty = y + PAD;
    line(buf, sw, sh, x + PAD, ty, w - 2 * PAD as u32, 14, "Monitor systemu", theme::TEXT_DARK);
    ty += LEADING + 4;

    let bars: &[(&str, u32)] = &[
        ("Procesor", 62),
        ("Pamięć", 41),
        ("Sieć", 18),
        ("Grafika (vgpu)", 78),
    ];
    for (label, pct) in bars {
        line(buf, sw, sh, x + PAD, ty, w - 2 * PAD as u32, 14, label, theme::TEXT_DARK);
        ty += LEADING - 4;
        let bar_w = w - 2 * PAD as u32;
        let bar_h = 12u32;
        draw_rect(buf, sw, sh, x + PAD, ty, bar_w, bar_h, theme::BORDER);
        let fill = bar_w * (*pct) / 100;
        fill_rect(buf, sw, sh, x + PAD + 1, ty + 1, fill - 1, bar_h - 2, theme::ACCENT);
        ty += bar_h as i32 + 8;
    }
}

// ── settings ─────────────────────────────────────────────────────────────────

fn draw_settings(buf: &mut [u32], sw: u32, sh: u32, x: i32, y: i32, w: u32, h: u32) {
    let _ = h;
    let mut ty = y + PAD;
    line(buf, sw, sh, x + PAD, ty, w - 2 * PAD as u32, 14, "Ustawienia", theme::TEXT_DARK);
    ty += LEADING + 4;

    // (label, toggle on/off)
    let rows: &[(&str, bool)] = &[
        ("Motyw: Breeze", true),
        ("Tapeta: gradient", true),
        ("Panel: dolny", true),
        ("Akcent: niebieski", true),
        ("Animacje: włączone", false),
        ("Język: polski", true),
    ];
    for (label, on) in rows {
        line(buf, sw, sh, x + PAD, ty, w - 2 * PAD as u32, 14, label, theme::TEXT_DARK);
        // Toggle pill on the right edge.
        let pill_w = 30u32;
        let pill_h = 16u32;
        let px = x + w as i32 - PAD - pill_w as i32;
        let pill = if *on { theme::ACCENT } else { theme::TEXT_MUTED };
        theme::fill_rounded_rect(buf, sw, sh, px, ty, pill_w, pill_h, 8, pill);
        ty += LEADING + 2;
    }
}

