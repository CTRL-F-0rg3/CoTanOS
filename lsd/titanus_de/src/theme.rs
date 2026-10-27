//! Kolory, typografia i geometria środowiska — kontrakt „wyglądu”.
//!
//! Ten moduł celowo jest **wyłącznie danymi**: żadnych funkcji rysujących,
//! żadnego stanu. Warstwa wyglądu (C#) autorsko definiuje te wartości i
//! serializuje je do JSON (`theme_cs/theme.json`), a Rust je wczytuje przez
//! [`Theme::from_json`] — C# ustawia pola, renderer w Rust tylko je czyta.
//!
//! Wartości domyślne ([`Theme::dark`]) to spójna, ciemna paleta zbieżna
//! z `uspace::gfx::app`; JSON generowany przez C# ma te same wartości.

use uspace::gfx::render::{rgb, Color};

/// Zestaw kolorów i wymiarów wyglądu.
///
/// Wszystkie pola są `pub`, bo to jest format wymiany, nie API z niezmiennikami.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub struct Theme {
    /// Kolor bazowy tła (spod tapety).
    pub background: Color,
    /// Lewy koniec gradientu zastępczego (gdy brak tapety).
    pub gradient_left: Color,
    /// Prawy koniec gradientu zastępczego.
    pub gradient_right: Color,
    /// Tło wnętrza okna.
    pub window_bg: Color,
    /// Obramowanie okna nieaktywnego.
    pub window_border: Color,
    /// Obramowanie okna skupionego.
    pub window_focus: Color,
    /// Tło paska tytułu.
    pub title_bg: Color,
    /// Tekst paska tytułu.
    pub title_text: Color,
    /// Tekst treści okna.
    pub text: Color,
    /// Tło doku po prawej.
    pub dock_bg: Color,
    /// Tło pojedynczej kafelki doku.
    pub dock_entry: Color,
    /// Tło kafelki aktywnej (podświetlenie fokusa).
    pub dock_active: Color,
    /// Tło paska systemowego.
    pub tray_bg: Color,
    /// Tekst paska systemowego.
    pub tray_text: Color,
    /// Rozmiar pisma etykiet (musi być w atlasie: 14/17/19).
    pub font_size: u32,
    /// Szerokość doku po prawej stronie, w pikselach.
    pub dock_width: u32,
    /// Odstęp paska systemowego od krawędzi ekranu.
    pub tray_margin: u32,
}

impl Theme {
    /// Domyslny, ciemny motyw.
    pub const fn dark() -> Self {
        Self {
            background: rgb(0x10, 0x12, 0x18),
            gradient_left: rgb(0x1A, 0x20, 0x2E),
            gradient_right: rgb(0x0D, 0x0E, 0x14),
            window_bg: rgb(0x1E, 0x22, 0x2C),
            window_border: rgb(0x3A, 0x44, 0x5A),
            window_focus: rgb(0x7C, 0xF0, 0x9C),
            title_bg: rgb(0x2A, 0x35, 0x4A),
            title_text: rgb(0xD0, 0xD8, 0xE0),
            text: rgb(0xC8, 0xD0, 0xDC),
            dock_bg: rgb(0x16, 0x18, 0x20),
            dock_entry: rgb(0x2A, 0x35, 0x4A),
            dock_active: rgb(0x7C, 0xF0, 0x9C),
            tray_bg: rgb(0x16, 0x18, 0x20),
            tray_text: rgb(0xD0, 0xD8, 0xE0),
            font_size: 14,
            dock_width: 64,
            tray_margin: 8,
        }
    }
}

impl Default for Theme {
    fn default() -> Self {
        Self::dark()
    }
}

/// Motyw domyślny wygenerowany przez C# (osadzony w binarkę w czasie
/// kompilacji). To „ciemny” motyw — te same wartości co [`Theme::dark`], ale
/// jedynym źródłem prawdy dla wyglądu jest JSON z `theme_cs`, a `dark()`
/// zostaje wyłącznie jako fallback wbudowany w kod.
pub const DEFAULT_THEME_JSON: &str = include_str!("../theme_cs/theme.json");

/// Surowy JSON motywu (format `theme_cs`). Pola kolorów to łańcuchy
/// `"#RRGGBB"`; [`Theme::from_json`] zamienia je na `Color`.
#[derive(serde::Deserialize)]
struct ThemeJson {
    background: String,
    gradient_left: String,
    gradient_right: String,
    window_bg: String,
    window_border: String,
    window_focus: String,
    title_bg: String,
    title_text: String,
    text: String,
    dock_bg: String,
    dock_entry: String,
    dock_active: String,
    tray_bg: String,
    tray_text: String,
    font_size: u32,
    dock_width: u32,
    tray_margin: u32,
}

impl Theme {
    /// Parsuje motyw z JSON (format produkowany przez `theme_cs`).
    pub fn from_json(json: &str) -> Result<Self, String> {
        let t: ThemeJson = serde_json::from_str(json).map_err(|e| e.to_string())?;
        Ok(Self {
            background: parse_hex_color(&t.background)?,
            gradient_left: parse_hex_color(&t.gradient_left)?,
            gradient_right: parse_hex_color(&t.gradient_right)?,
            window_bg: parse_hex_color(&t.window_bg)?,
            window_border: parse_hex_color(&t.window_border)?,
            window_focus: parse_hex_color(&t.window_focus)?,
            title_bg: parse_hex_color(&t.title_bg)?,
            title_text: parse_hex_color(&t.title_text)?,
            text: parse_hex_color(&t.text)?,
            dock_bg: parse_hex_color(&t.dock_bg)?,
            dock_entry: parse_hex_color(&t.dock_entry)?,
            dock_active: parse_hex_color(&t.dock_active)?,
            tray_bg: parse_hex_color(&t.tray_bg)?,
            tray_text: parse_hex_color(&t.tray_text)?,
            font_size: t.font_size,
            dock_width: t.dock_width,
            tray_margin: t.tray_margin,
        })
    }
}

/// Parsuje kolor `"#RRGGBB"` (albo `"#AARRGGBB"`) na `0xAARRGGBB`.
fn parse_hex_color(s: &str) -> Result<Color, String> {
    let t = s.trim().trim_start_matches('#');
    if t.len() == 6 {
        let v = u32::from_str_radix(t, 16).map_err(|_| format!("zły kolor: {s}"))?;
        Ok(0xFF00_0000 | v)
    } else if t.len() == 8 {
        u32::from_str_radix(t, 16).map_err(|_| format!("zły kolor: {s}"))
    } else {
        Err(format!("zły kolor: {s} (oczekiwano #RRGGBB lub #AARRGGBB)"))
    }
}

