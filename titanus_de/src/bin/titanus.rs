//! Demonstracyjny program Titanusa: buduje scenę i renderuje ramkę do PPM.
//!
//! Na hoście to jest droga diagnostyczna (plik `titanus_frame.ppm`), tak samo
//! jak `demo-gfx` i `settings-app`. Na systemie ten sam `Titanus::render`
//! rysowałby do framebuffera pobranego przez `uspace::kernel::KernelClient`.

use std::io::Write;

use titanus_de::{Fit, Point, Rect, Size, Theme, Titanus};

fn write_ppm(path: &str, fb: &[u32], w: u32, h: u32) -> std::io::Result<()> {
    let mut f = std::fs::File::create(path)?;
    write!(f, "P6\n{} {}\n255\n", w, h)?;

    let mut data = Vec::with_capacity((w * h * 3) as usize);
    for px in fb {
        data.push(((px >> 16) & 0xFF) as u8);
        data.push(((px >> 8) & 0xFF) as u8);
        data.push((px & 0xFF) as u8);
    }
    f.write_all(&data)?;
    Ok(())
}

fn main() {
    let mut size = Size::new(1280, 720);
    let mut wallpaper_path: Option<String> = None;
    let mut theme_path: Option<String> = None;
    let mut fit = Fit::Cover;

    let mut args = std::env::args().skip(1);
    while let Some(arg) = args.next() {
        match arg.as_str() {
            "--size" => {
                if let Some(v) = args.next() {
                    if let Some((w, h)) = v.split_once('x') {
                        if let (Ok(w), Ok(h)) = (w.parse::<u32>(), h.parse::<u32>()) {
                            size = Size::new(w, h);
                            continue;
                        }
                    }
                    eprintln!("[titanus] zły format --size (oczekiwano WxH): {v}");
                    std::process::exit(2);
                }
            }
            "--wallpaper" => {
                if let Some(v) = args.next() {
                    wallpaper_path = Some(v);
                } else {
                    eprintln!("[titanus] --wallpaper wymaga ścieżki");
                    std::process::exit(2);
                }
            }
            "--fit" => {
                if let Some(v) = args.next() {
                    fit = match v.as_str() {
                        "stretch" => Fit::Stretch,
                        "cover" => Fit::Cover,
                        "contain" => Fit::Contain,
                        "center" => Fit::Center,
                        "tile" => Fit::Tile,
                        other => {
                            eprintln!("[titanus] nieznany --fit: {other}");
                            std::process::exit(2);
                        }
                    };
                }
            }
            "--theme" => {
                if let Some(v) = args.next() {
                    theme_path = Some(v);
                } else {
                    eprintln!("[titanus] --theme wymaga ścieżki do pliku JSON");
                    std::process::exit(2);
                }
            }
            _ => {
                eprintln!("[titanus] nieznana opcja: {arg}");
                eprintln!("[titanus] uzycie: [--size WxH] [--wallpaper plik.png] [--theme motyw.json] [--fit cover|contain|stretch|center|tile]");
                std::process::exit(2);
            }
        }
    }

    let mut de = Titanus::new(size);

    // Motyw z C# (opcjonalny nadpisanie domyślnego).
    if let Some(path) = theme_path {
        match std::fs::read_to_string(&path) {
            Ok(json) => match Theme::from_json(&json) {
                Ok(theme) => {
                    de.theme = theme;
                    println!("[titanus] motyw wczytany: {path}");
                }
                Err(e) => eprintln!("[titanus] zły motyw {path}: {e}"),
            },
            Err(e) => eprintln!("[titanus] nie udało się odczytać {path}: {e}"),
        }
    }

    // Dwa kafelki + jedno okno pływające.
    de.spawn_tiled("Terminal");
    de.spawn_tiled("Edytor");
    de.spawn_floating("System", Rect::new(320, 180, 420, 260));
    // Fokus z powrotem na kafelek, żeby było widać oba obramowania.
    de.focus_prev();

    // Kursor gdzieś w obszarze roboczym (poza paskiem i doku).
    de.cursor = Point::new(120, 90);

    if let Some(path) = wallpaper_path {
        match std::fs::read(&path) {
            Ok(bytes) => match de.set_wallpaper(&bytes, fit) {
                Ok(()) => println!("[titanus] tapeta wczytana: {path}"),
                Err(e) => eprintln!("[titanus] nie udało się wczytać tapety {path}: {e}"),
            },
            Err(e) => eprintln!("[titanus] nie udało się odczytać {path}: {e}"),
        }
    }

    let mut fb = vec![0u32; (size.w * size.h) as usize];
    de.render(&mut fb);

    match write_ppm("titanus_frame.ppm", &fb, size.w, size.h) {
        Ok(()) => println!("[titanus] zapisano titanus_frame.ppm ({}x{})", size.w, size.h),
        Err(e) => eprintln!("[titanus] błąd zapisu: {e}"),
    }
}
