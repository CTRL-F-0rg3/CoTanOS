//! Desktop-session commands: launching the graphical environment.
//!
//! The actual desktop (floating windows, panel, launcher) is a separate `i3dw`
//! program in the user address space; this command is the in-system terminal's
//! hook for it. The same way `allde` switches input from the kernel terminal to
//! the tiling desktop, `i3dw` switches to the Plasma-like floating desktop.

use alloc::string::ToString;

use super::{CmdResult, Ctx};

// The kernel provides the interactive desktop; the shell command only hands
// control over and reports when the user returns. The desktop runs against the
// vgpu/HDMI framebuffer (`crate::gfx::console::try_fb`) with a dirty cursor,
// mouse and keyboard — the same way `allde` does, but Plasma-style.
extern "C" {
    fn i3dw_run();
    fn titanus_run();
}

/// `i3dw` — launch the KDE-Plasma-like floating desktop.
pub fn i3dw(_ctx: &mut Ctx<'_>, _args: &str) -> CmdResult {
    // The desktop loop blocks until ESC, then hands control back here.
    unsafe { i3dw_run(); }

    CmdResult::ok(vec![
        "i3dw — powrót z pulpitu (ESC)".to_string(),
        "klawisze: strzałki=kursor F4=klik Tab=fokus".to_string(),
        "         F1=minimalizuj F2=zamknij F3=nowe okno".to_string(),
    ])
}

/// `titanus` — launch the Titanus DE (kafelkowo-pływający, dok + pasek).
pub fn titanus(_ctx: &mut Ctx<'_>, _args: &str) -> CmdResult {
    // Rysuje pulpit Titanus DE i blokuje do naciśnięcia klawisza, potem wraca.
    unsafe { titanus_run(); }

    CmdResult::ok(vec![
        "titanus — powrót z pulpitu".to_string(),
        "klawisz powraca do powłoki".to_string(),
    ])
}
