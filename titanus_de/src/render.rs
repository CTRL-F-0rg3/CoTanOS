//! Renderer sceny: kompozycja warstw do pojedynczego framebuffera.
//!
//! Maluje warstwy w kolejności z [`crate::layers::Layer`]: tapeta → okna →
//! dok → pasek systemowy → kursor. Każda kolejna nadpisuje poprzednią, więc
//! z-order wynika wprost z kolejności wywołań — tak samo sortują powierzchnie
//! `uspace::gfx::Compositor` i `gfx-server::DisplayServer`.

use uspace::gfx::render::{draw_rect, fill_rect, rgb, Color};
use uspace::gfx::text::{self, Align};

use crate::de::Titanus;
use crate::geometry::{Point, Rect};
use crate::service::{ServiceManager, ServiceState};
use crate::theme::Theme;

/// Renderuje pełną ramkę sceny do `fb`.
pub fn render_scene(de: &Titanus, fb: &mut [u32]) {
    let w = de.screen.w;
    let h = de.screen.h;

    // 1. Tapeta (Layer::Wallpaper).
    match &de.wallpaper {
        Some(img) => crate::wallpaper::blit(img, fb, w, h, de.wallpaper_fit, de.theme.background),
        None => fill_gradient(fb, w, h, de.theme.gradient_left, de.theme.gradient_right),
    }

    // 2. Okna (Layer::Window) — kafelki najpierw, pływające na wierzchu.
    let placed = de.placed_windows();
    let focused = de.focused();
    for pl in &placed {
        let title = de.window_title(pl.id);
        render_window(fb, w, h, pl.rect, &title, focused == Some(pl.id), de.theme);
        // Okno "Usługi" to panel zarządzania daemonami i autostartem.
        if title == "Usługi" {
            render_services_panel(fb, w, h, pl.rect, &de.services, de.theme);
        }
    }

    // 3. Dok (Layer::Dock).
    render_dock(fb, de);

    // 4. Pasek systemowy (Layer::Tray).
    render_tray(fb, de);

    // 5. Kursor (Layer::Cursor).
    render_cursor(fb, w, h, de.cursor);
}

/// Okno: tło, obramowanie (kolor fokusa), pasek tytułu i etykieta.
fn render_window(fb: &mut [u32], w: u32, h: u32, r: Rect, title: &str, focused: bool, theme: Theme) {
    let border = if focused { theme.window_focus } else { theme.window_border };

    fill_rect(fb, w, h, r.x, r.y, r.w, r.h, theme.window_bg);
    draw_rect(fb, w, h, r.x, r.y, r.w, r.h, border);
    fill_rect(fb, w, h, r.x + 1, r.y + 1, r.w.saturating_sub(2), 22, theme.title_bg);
    text::draw_clipped(
        fb, w, h,
        r.x + 6, r.y + 4,
        theme.font_size, title, theme.title_text,
        r.w.saturating_sub(12),
        Align::Left,
    );
}

/// Panel „Usługi": lista daemonów z ich stanem i flagą autostartu.
///
/// Każda linia ma postać `[x] run  nazwa` — `[x]` to autostart, `run`/`stop`
/// to stan. To jest widok aplikacji do zarządzania autostartem i daemonami.
fn render_services_panel(fb: &mut [u32], w: u32, h: u32, r: Rect, services: &ServiceManager, theme: Theme) {
    let mut ty = r.y + 28;
    for svc in services.list() {
        let state = match svc.state {
            ServiceState::Stopped => "stop",
            ServiceState::Running => "run ",
            ServiceState::Failed => "fail",
        };
        let autostart = if svc.autostart { "[x]" } else { "[ ]" };
        let line = format!("{autostart} {state}  {}", svc.name);
        text::draw_clipped(
            fb, w, h,
            r.x + 8, ty,
            theme.font_size, &line, theme.text,
            r.w.saturating_sub(16),
            Align::Left,
        );
        ty += 18;
    }
}

/// Dok po prawej: tło + kwadratowe kafelki aplikacji z etykietą (pierwsza litera).
fn render_dock(fb: &mut [u32], de: &Titanus) {
    let (w, h) = (de.screen.w, de.screen.h);
    let theme = de.theme;
    let dock_rect = de.dock.rect(de.screen, &theme);
    fill_rect(fb, w, h, dock_rect.x, dock_rect.y, dock_rect.w, dock_rect.h, theme.dock_bg);

    for (i, entry) in de.dock.entries.iter().enumerate() {
        let r = de.dock.entry_rect(i, de.screen, &theme);
        let color = if de.dock.focused == Some(entry.id) {
            theme.dock_active
        } else {
            theme.dock_entry
        };
        fill_rounded_rect(fb, w, h, r, 8, color);

        let label: String = entry.name.chars().next().map(|c| c.to_string()).unwrap_or_default();
        draw_label_centered(fb, w, h, r, theme.font_size, &label, theme.title_text);
    }
}

/// Pasek systemowy: „godzina  głośność  sieć” w jednej pigułce.
fn render_tray(fb: &mut [u32], de: &Titanus) {
    let (w, h) = (de.screen.w, de.screen.h);
    let theme = de.theme;
    let r = de.tray.rect(&theme);

    fill_rounded_rect(fb, w, h, r, 17, theme.tray_bg);

    let network = match de.tray.network {
        crate::tray::NetworkState::Offline => "offline".to_string(),
        crate::tray::NetworkState::Wifi(signal) => format!("wifi {signal}%"),
        crate::tray::NetworkState::Ethernet => "ethernet".to_string(),
    };
    let volume = if de.tray.muted {
        "mute".to_string()
    } else {
        format!("vol {}%", de.tray.volume)
    };

    let line = format!("{}   {}   {}", de.tray.clock, volume, network);
    text::draw_clipped(
        fb, w, h,
        r.x + 12, r.y + 10,
        theme.font_size, &line, theme.tray_text,
        r.w.saturating_sub(24),
        Align::Left,
    );
}

/// Kursor: prosty celownik (krzyżyk).
fn render_cursor(fb: &mut [u32], w: u32, h: u32, p: Point) {
    let c = rgb(0xFF, 0xFF, 0xFF);
    fill_rect(fb, w, h, p.x, p.y - 4, 1, 9, c);
    fill_rect(fb, w, h, p.x - 4, p.y, 9, 1, c);
}

/// Etykieta wyśrodkowana w pionie i poziomie wewnątrz pola.
fn draw_label_centered(fb: &mut [u32], w: u32, h: u32, r: Rect, size: u32, text: &str, color: Color) {
    let asc = text::ascent(size).unwrap_or(size);
    let desc = text::descent(size).unwrap_or(0);
    let line = asc + desc;
    let top = r.y + (r.h as i32 - line as i32) / 2;
    text::draw_clipped(fb, w, h, r.x, top, size, text, color, r.w, Align::Center);
}

/// Wypełnia framebuffer gradientem poziomym `left` → `right`.
fn fill_gradient(fb: &mut [u32], w: u32, h: u32, left: Color, right: Color) {
    if w == 0 {
        return;
    }
    for x in 0..w {
        let t = x as u64 * 255 / (w as u64 - 1);
        let c = lerp(left, right, t as u32);
        for y in 0..h {
            fb[(y * w + x) as usize] = c;
        }
    }
}

/// Miesza dwa kolory w proporcji `t` z zakresu 0..=255.
fn lerp(a: Color, b: Color, t: u32) -> Color {
    let t = t.min(255);
    let inv = 255 - t;
    let r = (((a >> 16) & 0xFF) * inv + ((b >> 16) & 0xFF) * t) / 255;
    let g = (((a >> 8) & 0xFF) * inv + ((b >> 8) & 0xFF) * t) / 255;
    let bl = ((a & 0xFF) * inv + (b & 0xFF) * t) / 255;
    0xFF00_0000 | (r << 16) | (g << 8) | bl
}

/// Wypełnia prostokąt z zaokrąglonymi rogami (promień przycinany do połowy
/// krótszego boku, więc maksymalny daje kapsułkę).
fn fill_rounded_rect(fb: &mut [u32], w: u32, h: u32, r: Rect, radius: u32, color: Color) {
    if r.w == 0 || r.h == 0 {
        return;
    }
    let radius = radius.min(r.w / 2).min(r.h / 2);

    for row in 0..r.h {
        let py = r.y + row as i32;
        let into_corner = if row < radius {
            radius - row
        } else if row >= r.h - radius {
            row - (r.h - radius) + 1
        } else {
            0
        };

        let inset = if into_corner == 0 {
            0
        } else {
            let dy = into_corner - 1;
            radius - isqrt(radius * radius - dy * dy)
        };

        let x0 = r.x + inset as i32;
        let x1 = r.x + r.w as i32 - inset as i32;
        if x1 > x0 {
            fill_rect(fb, w, h, x0, py, (x1 - x0) as u32, 1, color);
        }
    }
}

/// Pierwiastek całkowity z zaokrągleniem w dół (wartości są małe, więc pętla
/// jest tańsza i pewniejsza niż floaty).
fn isqrt(v: u32) -> u32 {
    if v == 0 {
        return 0;
    }
    let mut guess = 1u32;
    while (guess + 1) * (guess + 1) <= v {
        guess += 1;
    }
    guess
}

