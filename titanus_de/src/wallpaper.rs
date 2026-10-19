//! Tapety PNG: dekodowanie i dopasowanie do ekranu.
//!
//! # Dekodowanie
//!
//! `decode_png` używa skrzynki `png` (czysto rustowej), normalizuje wejście do
//! 8-bitowego koloru z alfą (`normalize_to_color8`) i zwraca bufor RGBA8. To
//! jedyny moduł Titanusa, który dotyka zależności zewnętrznej — reszta rdzenia
//! operuje na gotowym [`DecodedImage`].
//!
//! # Dopasowanie
//!
//! [`Fit`] opisuje, jak obraz ma wypełnić ekran: rozciągnięcie, pokrycie z
//! przycięciem, zmieszczenie z paskami, wycentrowanie 1:1 albo kafelkowanie.
//! Skalowanie jest najbliższym sąsiadem w arytmetyce całkowitej — bez floatów,
//! więc deterministyczne i identyczne na każdym CPU.

use std::io::Cursor;

use crate::geometry::{Point, Rect};

/// Zdekodowany obraz: bufor RGBA8, wiersz po wierszu.
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct DecodedImage {
    pub width: u32,
    pub height: u32,
    /// `width * height` pikseli, każdy `[R, G, B, A]`.
    pub pixels: Vec<[u8; 4]>,
}

/// Sposób dopasowania tapety do ekranu.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum Fit {
    /// Rozciąga do pełnego ekranu, ignorując proporcje.
    Stretch,
    /// Skaluje tak, by pokryć ekran; nadmiar przycięty, proporcje zachowane.
    Cover,
    /// Skaluje tak, by całość się zmieściła; paski po bokach.
    Contain,
    /// Wyśrodkowuje w rozmiarze 1:1.
    Center,
    /// Kafelkuje w rozmiarze 1:1.
    Tile,
}

/// Dekoduje plik PNG do bufora RGBA8.
pub fn decode_png(bytes: &[u8]) -> Result<DecodedImage, String> {
    let mut decoder = png::Decoder::new(Cursor::new(bytes));
    decoder.set_transformations(png::Transformations::normalize_to_color8());
    let mut reader = decoder.read_info().map_err(|e| e.to_string())?;

    let mut buf = vec![0u8; reader.output_buffer_size()];
    let info = reader.next_frame(&mut buf).map_err(|e| e.to_string())?;

    let width = info.width;
    let height = info.height;
    let mut pixels = Vec::with_capacity((width as usize) * (height as usize));

    // `normalize_to_color8` (STRIP_16 | EXPAND) rozwija paletę do RGB i dodaje
    // alfa z tRNS, więc po dekodowaniu zostają wyłącznie te cztery formaty.
    match info.color_type {
        png::ColorType::Grayscale => {
            for px in buf.chunks_exact(1) {
                let v = px[0];
                pixels.push([v, v, v, 255]);
            }
        }
        png::ColorType::GrayscaleAlpha => {
            for px in buf.chunks_exact(2) {
                pixels.push([px[0], px[0], px[0], px[1]]);
            }
        }
        png::ColorType::Rgb => {
            for px in buf.chunks_exact(3) {
                pixels.push([px[0], px[1], px[2], 255]);
            }
        }
        png::ColorType::Rgba => {
            for px in buf.chunks_exact(4) {
                pixels.push([px[0], px[1], px[2], px[3]]);
            }
        }
        png::ColorType::Indexed => {
            return Err("indeksowany PNG nie został rozwinięty do RGB".to_string());
        }
    }

    Ok(DecodedImage { width, height, pixels })
}

/// Rysuje obraz na framebufferze `dst` (wymiary `dst_w` × `dst_h`) wg [`Fit`].
///
/// Cały `dst` najpierw wypełnia `background` — to baza pod alfę i paski
/// w trybach Contain/Center.
pub fn blit(
    img: &DecodedImage,
    dst: &mut [u32],
    dst_w: u32,
    dst_h: u32,
    fit: Fit,
    background: u32,
) {
    for px in dst.iter_mut() {
        *px = background;
    }
    if img.width == 0 || img.height == 0 {
        return;
    }

    match fit {
        Fit::Tile => blit_tile(img, dst, dst_w, dst_h),
        other => {
            let draw = fit_rect(img.width, img.height, dst_w, dst_h, other);
            blit_scaled(img, dst, dst_w, dst_h, draw);
        }
    }
}

/// Prostokąt, w którym ma się znaleźć obraz (może wystawać poza ekran dla Cover).
fn fit_rect(img_w: u32, img_h: u32, dst_w: u32, dst_h: u32, fit: Fit) -> Rect {
    match fit {
        Fit::Stretch => Rect::new(0, 0, dst_w, dst_h),
        Fit::Cover => {
            let (dw, dh) = scaled(img_w, img_h, dst_w, dst_h, true);
            centered(dst_w, dst_h, dw, dh)
        }
        Fit::Contain => {
            let (dw, dh) = scaled(img_w, img_h, dst_w, dst_h, false);
            centered(dst_w, dst_h, dw, dh)
        }
        Fit::Center => centered(dst_w, dst_h, img_w, img_h),
        Fit::Tile => Rect::new(0, 0, dst_w, dst_h),
    }
}

fn centered(screen_w: u32, screen_h: u32, w: u32, h: u32) -> Rect {
    Rect::new(
        (screen_w as i32 - w as i32) / 2,
        (screen_h as i32 - h as i32) / 2,
        w,
        h,
    )
}

/// Rozmiar, do którego przeskalować obraz, by pokrył (`cover`) albo zmieścił
/// się (`!cover`) na ekranie, zachowując proporcje. Liczone na `u64` przez
/// mnożenie na krzyż, bez dzielenia ułamkowego.
fn scaled(img_w: u32, img_h: u32, dst_w: u32, dst_h: u32, cover: bool) -> (u32, u32) {
    let (iw, ih) = (img_w as u64, img_h as u64);
    let (dw, dh) = (dst_w as u64, dst_h as u64);

    // Obraz jest relatywnie "szerszy" niż ekran, gdy iw/ih > dw/dh.
    let wider = iw * dh > ih * dw;

    if cover {
        if wider {
            // Wysokość ogranicza; szerokość wystaje (przycinana).
            (ceil_div(iw * dh, ih) as u32, dh as u32)
        } else {
            (dw as u32, ceil_div(ih * dw, iw) as u32)
        }
    } else if wider {
        (dw as u32, (ih * dw / iw) as u32)
    } else {
        ((iw * dh / ih) as u32, dh as u32)
    }
}

fn ceil_div(a: u64, b: u64) -> u64 {
    (a + b - 1) / b
}

/// Rysuje obraz przeskalowany (najbliższy sąsiad) do prostokąta `draw`.
fn blit_scaled(img: &DecodedImage, dst: &mut [u32], dst_w: u32, dst_h: u32, draw: Rect) {
    if draw.w == 0 || draw.h == 0 {
        return;
    }
    for dy in 0..dst_h {
        for dx in 0..dst_w {
            let p = Point::new(dx as i32, dy as i32);
            if !draw.contains(p) {
                continue;
            }
            let sx = ((dx as i32 - draw.x) as u64 * img.width as u64 / draw.w as u64) as u32;
            let sy = ((dy as i32 - draw.y) as u64 * img.height as u64 / draw.h as u64) as u32;
            let src = img.pixels[(sy * img.width + sx) as usize];
            let i = (dy * dst_w + dx) as usize;
            dst[i] = blend(dst[i], src);
        }
    }
}

/// Rysuje obraz kafelkowany w rozmiarze 1:1.
fn blit_tile(img: &DecodedImage, dst: &mut [u32], dst_w: u32, dst_h: u32) {
    for dy in 0..dst_h {
        for dx in 0..dst_w {
            let sx = (dx % img.width) as usize;
            let sy = (dy % img.height) as usize;
            let src = img.pixels[sy * img.width as usize + sx];
            let i = (dy * dst_w + dx) as usize;
            dst[i] = blend(dst[i], src);
        }
    }
}

/// Miesza piksel źródłowy (RGBA) nad docelowym (`0xAARRGGBB`) — standardowy
/// alpha-over w arytmetyce całkowitej.
fn blend(dst: u32, src: [u8; 4]) -> u32 {
    let a = src[3] as u32;
    if a == 0 {
        return dst;
    }
    if a == 255 {
        return 0xFF00_0000
            | ((src[0] as u32) << 16)
            | ((src[1] as u32) << 8)
            | (src[2] as u32);
    }
    let dr = (dst >> 16) & 0xFF;
    let dg = (dst >> 8) & 0xFF;
    let db = dst & 0xFF;
    let r = (src[0] as u32 * a + dr * (255 - a)) / 255;
    let g = (src[1] as u32 * a + dg * (255 - a)) / 255;
    let b = (src[2] as u32 * a + db * (255 - a)) / 255;
    0xFF00_0000 | (r << 16) | (g << 8) | b
}

