//! Testy: rdzeń Rust (geometria, warstwy, tapety, dok) + logika C++ przez FFI.

use super::*;

// ── geometria ────────────────────────────────────────────────────────────────

#[test]
fn rect_contains_half_open() {
    let r = Rect::new(10, 20, 30, 40);
    assert!(r.contains(Point::new(10, 20)));
    assert!(r.contains(Point::new(39, 59)));
    assert!(!r.contains(Point::new(40, 59))); // prawa krawędź wyłączna
    assert!(!r.contains(Point::new(39, 60))); // dolna krawędź wyłączna
    assert!(!r.contains(Point::new(9, 20)));
}

// ── warstwy ──────────────────────────────────────────────────────────────────

#[test]
fn layer_order_is_stable_and_ascending() {
    let mut all = Layer::ALL.to_vec();
    all.sort();
    assert_eq!(all, Layer::ALL.to_vec());

    assert!(Layer::Wallpaper < Layer::Window);
    assert!(Layer::Window < Layer::Dock);
    assert!(Layer::Dock < Layer::Tray);
    assert!(Layer::Tray < Layer::Cursor);
}

// ── tapety PNG ───────────────────────────────────────────────────────────────

#[test]
fn decode_png_roundtrip() {
    let mut bytes = Vec::new();
    {
        let mut enc = png::Encoder::new(&mut bytes, 2, 2);
        enc.set_color(png::ColorType::Rgba);
        enc.set_depth(png::BitDepth::Eight);
        let mut writer = enc.write_header().unwrap();
        writer
            .write_image_data(&[
                255, 0, 0, 255, //
                0, 255, 0, 255, //
                0, 0, 255, 255, //
                255, 255, 255, 255,
            ])
            .unwrap();
    }

    let img = decode_png(&bytes).unwrap();
    assert_eq!((img.width, img.height), (2, 2));
    assert_eq!(
        img.pixels,
        vec![
            [255, 0, 0, 255],
            [0, 255, 0, 255],
            [0, 0, 255, 255],
            [255, 255, 255, 255],
        ]
    );
}

#[test]
fn blit_stretch_maps_corners() {
    let img = DecodedImage {
        width: 2,
        height: 2,
        pixels: vec![
            [255, 0, 0, 255],
            [0, 255, 0, 255],
            [0, 0, 255, 255],
            [255, 255, 255, 255],
        ],
    };
    let mut fb = vec![0u32; 4 * 4];
    wallpaper::blit(&img, &mut fb, 4, 4, Fit::Stretch, 0xFF00_0000);

    assert_eq!(fb[0], 0xFFFF_0000);
    assert_eq!(fb[3], 0xFF00_FF00);
    assert_eq!(fb[12], 0xFF00_00FF);
    assert_eq!(fb[15], 0xFFFF_FFFF);
}

#[test]
fn blit_cover_crops_overflow() {
    let img = DecodedImage {
        width: 2,
        height: 1,
        pixels: vec![[255, 0, 0, 255], [0, 255, 0, 255]],
    };
    let mut fb = vec![0u32; 16];
    wallpaper::blit(&img, &mut fb, 4, 4, Fit::Cover, 0xFF00_0000);

    for &px in &fb {
        assert!(px == 0xFFFF_0000 || px == 0xFF00_FF00, "tło: {px:#x}");
    }
    assert_eq!(fb[0], 0xFFFF_0000);
    assert_eq!(fb[3], 0xFF00_FF00);
}

#[test]
fn blit_contain_letterboxes() {
    let img = DecodedImage {
        width: 1,
        height: 2,
        pixels: vec![[255, 0, 0, 255], [0, 0, 255, 255]],
    };
    let bg = 0xFF10_1010;
    let mut fb = vec![0u32; 16];
    wallpaper::blit(&img, &mut fb, 4, 4, Fit::Contain, bg);

    assert_eq!(fb[0], bg);
    assert_eq!(fb[3], bg);
    assert_eq!(fb[1], 0xFFFF_0000);
    assert_eq!(fb[1 + 3 * 4], 0xFF00_00FF);
}

// ── dok ──────────────────────────────────────────────────────────────────────

#[test]
fn dock_hit_test() {
    let mut dock = Dock::new();
    let id = dock.add("Terminal");
    let theme = Theme::dark();
    let screen = Size::new(800, 600);

    let r = dock.entry_rect(0, screen, &theme);
    assert_eq!(dock.hit(Point::new(r.x + 5, r.y + 5), screen, &theme), Some(id));
    assert_eq!(dock.hit(Point::new(0, 0), screen, &theme), None);
}

// ── logika C++ (DeLogic) ─────────────────────────────────────────────────────

#[test]
fn logic_spawn_focus_and_title() {
    let mut l = DeLogic::new();
    let a = l.spawn_tiled("a");
    let b = l.spawn_tiled("b");

    assert_eq!(l.focused(), Some(b));
    l.focus_next();
    assert_eq!(l.focused(), Some(a));
    l.focus_prev();
    assert_eq!(l.focused(), Some(b));

    assert_eq!(l.window_title(a), "a");
    assert_eq!(l.window_count(), 2);
    assert_eq!(l.workspace_count(), 1);
}

#[test]
fn logic_layout_tiling_and_floating() {
    let mut l = DeLogic::new();
    l.spawn_tiled("a");
    l.spawn_tiled("b");

    let placed = l.layout(Rect::new(0, 0, 100, 50), 8);
    assert_eq!(placed.len(), 2);
    assert_eq!(placed[0].rect.w, 46);
    assert_eq!(placed[0].rect.x, 0);
    assert_eq!(placed[1].rect.x, 54);

    l.spawn_floating("f", Rect::new(200, 200, 100, 100));
    let placed = l.layout(Rect::new(0, 0, 800, 600), 8);
    assert_eq!(placed.len(), 3);
    assert_eq!(placed[2].kind, WindowKind::Floating);
    assert_eq!(placed[2].rect, Rect::new(200, 200, 100, 100));
}

#[test]
fn logic_workspaces_and_close() {
    let mut l = DeLogic::new();
    let a = l.spawn_tiled("a");
    l.move_window_to_workspace(a, 1);

    assert_eq!(l.workspace_count(), 2);
    l.close_window(a);
    assert_eq!(l.window_count(), 0);
    assert_eq!(l.focused(), None);
}

// ── środowisko ───────────────────────────────────────────────────────────────

#[test]
fn de_spawn_layout_and_focus() {
    let mut de = Titanus::new(Size::new(800, 600));
    de.spawn_tiled("a");
    let b = de.spawn_tiled("b");

    assert_eq!(de.focused(), Some(b));

    let placed = de.placed_windows();
    assert_eq!(placed.len(), 2);
    // work area = 800 - 64 dok = 736; (736 - 8) / 2 = 364 na kolumnę.
    assert_eq!(placed[0].rect.w, 364);
    assert_eq!(placed[0].rect.x, 0);
    assert_eq!(placed[1].rect.x, 372);

    de.focus_next();
    assert_eq!(de.focused(), Some(1));
    de.focus_prev();
    assert_eq!(de.focused(), Some(b));
}

#[test]
fn de_workspaces_and_close() {
    let mut de = Titanus::new(Size::new(800, 600));
    let a = de.spawn_tiled("a");
    de.move_window_to_workspace(a, 1);

    assert_eq!(de.workspace_count(), 2);
    de.close_window(a);
    assert_eq!(de.focused(), None);
}

#[test]
fn de_window_at_floating_wins() {
    let mut de = Titanus::new(Size::new(800, 600));
    de.spawn_tiled("a");
    let f = de.spawn_floating("f", Rect::new(300, 300, 200, 200));

    assert_eq!(de.window_at(Point::new(350, 350)), Some(f));
    assert!(de.window_at(Point::new(10, 10)).is_some());
}

#[test]
fn render_smoke_writes_framebuffer() {
    let mut de = Titanus::new(Size::new(320, 200));
    de.spawn_tiled("a");
    de.spawn_floating("f", Rect::new(40, 40, 100, 80));

    let mut fb = vec![0u32; (320 * 200) as usize];
    de.render(&mut fb);

    assert_ne!(fb[0], 0);
    assert_eq!(fb.len(), (320 * 200) as usize);
}

#[test]
fn de_wallpaper_renders_over_scene() {
    let mut bytes = Vec::new();
    {
        let mut enc = png::Encoder::new(&mut bytes, 1, 1);
        enc.set_color(png::ColorType::Rgba);
        enc.set_depth(png::BitDepth::Eight);
        let mut writer = enc.write_header().unwrap();
        writer.write_image_data(&[255, 0, 0, 255]).unwrap();
    }

    let mut de = Titanus::new(Size::new(200, 100));
    de.set_wallpaper(&bytes, Fit::Stretch).unwrap();
    // Kursor poza ekranem, żeby nie zamalowywał badanego piksela.
    de.cursor = Point::new(-10, -10);

    let mut fb = vec![0u32; (200 * 100) as usize];
    de.render(&mut fb);

    assert_eq!(fb[0], 0xFFFF_0000);
    assert_eq!(fb[60 * 200 + 50], 0xFFFF_0000);
}

#[test]
fn theme_from_csharp_json_matches_dark() {
    // Motyw osadzony z C# (theme_cs/theme.json) musi dawać te same wartości co
    // wbudowany `Theme::dark()` — dowód, że pipeline C# -> JSON -> Rust działa.
    let from_cs = Theme::from_json(DEFAULT_THEME_JSON).unwrap();
    assert_eq!(from_cs, Theme::dark());
    assert_eq!(from_cs.background, 0xFF10_1218);
}

// ── usługi (daemony) i autostart ────────────────────────────────────────────

#[test]
fn service_manager_add_start_stop_autostart() {
    let mut m = ServiceManager::new();
    let id = m.add("network_manager", true);

    assert_eq!(m.list().len(), 1);
    assert_eq!(m.list()[0].state, ServiceState::Stopped);
    assert_eq!(m.autostart_count(), 1);

    assert!(m.start(id));
    assert_eq!(m.list()[0].state, ServiceState::Running);
    assert_eq!(m.running_count(), 1);

    assert!(m.stop(id));
    assert_eq!(m.list()[0].state, ServiceState::Stopped);

    assert!(m.restart(id));
    assert_eq!(m.list()[0].state, ServiceState::Running);

    assert!(m.set_autostart(id, false));
    assert_eq!(m.autostart_count(), 0);

    assert!(m.remove(id));
    assert!(m.list().is_empty());
    assert!(!m.remove(id)); // już usunięta
}

#[test]
fn service_manager_default_has_daemons() {
    let m = ServiceManager::default();
    assert_eq!(m.list().len(), 5);
    assert_eq!(m.autostart_count(), 5); // domyślnie wszystkie z autostartem
}

#[test]
fn service_manager_invalid_id() {
    let mut m = ServiceManager::new();
    assert!(!m.start(999));
    assert!(!m.stop(999));
    assert!(!m.set_autostart(999, true));
    assert!(m.list().get(999).is_none());
}

#[test]
fn de_has_default_services_and_renders_panel() {
    let mut de = Titanus::new(Size::new(400, 300));
    assert_eq!(de.services.list().len(), 5);

    // Okno „Usługi" renderuje panel z listą daemonów.
    de.spawn_floating("Usługi", Rect::new(20, 20, 360, 260));
    let mut fb = vec![0u32; (400 * 300) as usize];
    de.render(&mut fb);
    assert_ne!(fb[0], 0);
}

