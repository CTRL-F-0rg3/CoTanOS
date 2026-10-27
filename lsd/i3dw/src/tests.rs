//! Host-side tests for the `i3dw` desktop: the window manager invariants and
//! an end-to-end frame render.

use crate::{AppKind, I3dw};

#[test]
fn focus_raises_window_to_top() {
    let mut de = I3dw::new(800, 600);
    let a = de.spawn_app(AppKind::Terminal);
    let b = de.spawn_app(AppKind::Welcome);
    assert_ne!(a, b);

    // `b` was spawned last and is focused; focusing `a` must raise it above.
    de.wm.focus(a);
    let top = de
        .wm
        .layout(crate::theme::PANEL_H)
        .last()
        .map(|(id, _)| *id);
    assert_eq!(top, Some(a), "the focused window must be topmost");
}

#[test]
fn minimize_hides_window_from_layout() {
    let mut de = I3dw::new(800, 600);
    let a = de.spawn_app(AppKind::Terminal);
    de.spawn_app(AppKind::Welcome);

    de.wm.minimize(a);
    let ids: Vec<_> = de
        .wm
        .layout(crate::theme::PANEL_H)
        .into_iter()
        .map(|(id, _)| id)
        .collect();
    assert!(!ids.contains(&a), "a minimized window must not be laid out");
}

#[test]
fn maximize_fills_work_area_above_panel() {
    let mut de = I3dw::new(1280, 800);
    let a = de.spawn_app(AppKind::Terminal);

    de.wm.toggle_maximize(a, crate::theme::PANEL_H);
    let r = de
        .wm
        .layout(crate::theme::PANEL_H)
        .into_iter()
        .find(|(id, _)| *id == a)
        .map(|(_, r)| r)
        .unwrap();

    assert_eq!(r.x, 0);
    assert_eq!(r.y, 0);
    assert_eq!(r.w, 1280);
    assert_eq!(r.h, 800 - crate::theme::PANEL_H);
}

#[test]
fn dirty_cursor_draws_moves_and_restores() {
    let (w, h) = (200u32, 200u32);
    let bg = 0x1122_3344u32;
    let mut fb = vec![bg; (w * h) as usize];
    let mut cur = crate::DirtyCursor::new(w, h);
    cur.x = 50;
    cur.y = 50;

    // Draw the cursor: it must change pixels under it.
    cur.attach(&mut fb, w, h);
    assert!(
        fb.iter().any(|&p| p != bg),
        "attaching the cursor must draw the arrow"
    );

    // Move: only the cursor's box is touched, never the whole frame.
    let far = fb[0];
    cur.move_to(&mut fb, w, h, 100, 100);
    assert_eq!(far, fb[0], "a cursor move must not rewrite unrelated pixels");
    assert_eq!(fb[(50 * w + 50) as usize], bg, "the old position is restored on move");

    // Detach: the new position is restored too.
    cur.detach(&mut fb, w, h);
    assert_eq!(fb[(100 * w + 100) as usize], bg, "detach restores the covered pixels");
}

#[test]
fn render_live_overlays_cursor_on_the_desktop() {
    let (w, h) = (400u32, 300u32);
    let mut de = I3dw::new(w, h);
    de.spawn_app(AppKind::Terminal);

    let mut fb = vec![0u32; (w * h) as usize];
    de.render_live(&mut fb);

    // The cursor box contains at least one white fill pixel.
    let cur = &de.cursor;
    let has_cursor = fb
        .iter()
        .enumerate()
        .any(|(i, &p)| p == 0xFFFF_FFFF && i >= ((cur.y * w + cur.x) as usize));
    assert!(has_cursor, "render_live must draw the white cursor over the desktop");
}

#[test]
fn desktop_renders_wallpaper_panel_window_and_launcher() {
    let (w, h) = (800u32, 600u32);
    let mut de = I3dw::new(w, h);
    de.clock = "12:00".to_string();
    de.date = "test".to_string();
    de.spawn_app(AppKind::Terminal);
    de.spawn_app(AppKind::Settings);
    de.toggle_launcher();

    let mut fb = vec![0u32; (w * h) as usize];
    de.render(&mut fb);

    // The wallpaper top-left pixel is the gradient start, never zero.
    assert_ne!(fb[0], 0, "wallpaper must be drawn");
    assert!(
        fb.iter().any(|&p| p == crate::theme::PANEL_BG),
        "panel background must be present"
    );
    assert!(
        fb.iter().any(|&p| p == crate::theme::WIN_BG),
        "window body must be present"
    );
    assert!(
        fb.iter().any(|&p| p == crate::theme::LAUNCHER_BG),
        "launcher popup must be present"
    );
}
