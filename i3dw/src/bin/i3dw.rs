//! `i3dw` — a scripted session of the Plasma-like desktop environment.
//!
//! Spawns several apps (welcome, terminal, file manager, system monitor,
//! settings), raises the welcome window, opens the launcher, then renders the
//! desktop frame to `i3dw_frame.ppm`.

use i3dw::{AppKind, I3dw};

fn write_ppm(path: &str, fb: &[u32], w: u32, h: u32) -> std::io::Result<()> {
    use std::io::Write;
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
    let (w, h) = (1280u32, 800u32);
    let mut fb = vec![0u32; (w * h) as usize];
    let mut de = I3dw::new(w, h);
    de.clock = "14:32".to_string();
    de.date = "18 paź 2026".to_string();

    let welcome = de.spawn_app(AppKind::Welcome);
    de.spawn_app(AppKind::Terminal);
    de.spawn_app(AppKind::FileManager);
    de.spawn_app(AppKind::SystemMonitor);
    de.spawn_app(AppKind::Settings);

    // Raise the welcome window above the cascade, then open the launcher so the
    // frame shows the application menu as well as the floating windows.
    de.wm.focus(welcome);
    de.toggle_launcher();

    de.render(&mut fb);
    write_ppm("i3dw_frame.ppm", &fb, w, h).expect("write ppm");
    println!(
        "[i3dw] rendered i3dw_frame.ppm ({}x{}, {} windows, launcher {})",
        w,
        h,
        de.wm.count(),
        if de.launcher_open { "open" } else { "closed" }
    );
}
