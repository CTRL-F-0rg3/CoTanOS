//! Automatic display-mode selection at boot.
//!
//! # Why this is automatic
//!
//! This used to be a Redox-style pre-login resolution chooser drawn as a TUI
//! into the framebuffer. The menu is gone, along with the `tui` drawing layer it
//! was the only consumer of: a machine that boots a 1920x1080 desktop should
//! reach the login prompt without stopping to ask which of nine modes to use.
//! The one mode that matters is applied here, without waiting for input, so an
//! unattended machine still reaches the login prompt.
//!
//! The mode is set *before* the login prompt for the same reason the old menu
//! chose first: the text is laid out once, at the size it will keep, rather than
//! being re-laid-out after login.

use crate::fs::session;
use crate::gfx;

/// The display mode the kernel boots into.
///
/// 1920x1080 is the mode the rest of the boot path expects: it is what the
/// base `config.tcfg` seeds as `resolution=1920x1080`, and it is the mode the
/// login prompt and shell are laid out for.
pub const DEFAULT_W: u32 = 1920;
pub const DEFAULT_H: u32 = 1080;

/// Apply the default display mode and return the mode actually in effect.
///
/// No input is read and nothing is drawn: the choice is made here and applied.
/// A mode set runs through the BIOS video path, which may leave the 8042 (PS/2)
/// controller disabled, so the input path is re-asserted afterwards — otherwise
/// the login prompt below would sit behind a dead keyboard.
///
/// Returns `(1920, 1080)` when the mode takes, or the current resolution when
/// the display refuses it (for example on hardware with no Bochs VBE LFB).
pub fn run() -> (u32, u32) {
    let (cur_w, cur_h) = gfx::current_resolution();

    let applied = if cur_w == DEFAULT_W && cur_h == DEFAULT_H {
        // Already there. Saying so beats doing a pointless mode set, which
        // would clear the screen and force a redraw for no reason at all.
        session::say(&alloc::format!("display: keeping {DEFAULT_W}x{DEFAULT_H}\n"));
        (cur_w, cur_h)
    } else if gfx::set_resolution_w_h(DEFAULT_W, DEFAULT_H) {
        session::say(&alloc::format!("display: {DEFAULT_W}x{DEFAULT_H}\n"));
        (DEFAULT_W, DEFAULT_H)
    } else {
        session::say(&alloc::format!(
            "display: {DEFAULT_W}x{DEFAULT_H} refused by this display, staying at {cur_w}x{cur_h}\n"
        ));
        (cur_w, cur_h)
    };

    // A display-mode set can leave the 8042 (PS/2) keyboard interface disabled,
    // which would strand the login prompt with a dead keyboard. Re-assert it
    // unconditionally so the keyboard is never locked when login reads it.
    crate::terminal::restore_input();

    applied
}







