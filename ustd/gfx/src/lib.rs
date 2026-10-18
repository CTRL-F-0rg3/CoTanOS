//! Software raster primitives for the TrangorgeOS ring-3 framebuffer.
//!
//! `tgs-gfx` is the thin layer of drawing code between a program and the
//! kernel-provided framebuffer ([`tgs_hal::ui::Framebuffer`]). It is pure
//! integer, allocation-free, and depends only on `tgs-hal`, so it works on a
//! `no_std` program or a full `std` one alike.

#![no_std]

pub mod render;

pub use render::{
    draw_circle, draw_line, draw_rect, draw_text, fill_circle, fill_rect, hue,
    open_canvas, rgb, Canvas, Color,
};

/// Keep the calling world alive, yielding to the kernel between spins.
///
/// A graphical program ends here when it has nothing left to redraw (or no
/// framebuffer at all): the world stays scheduled, and the kernel gets a chance
/// to poll its rings on every tick.
pub fn idle() -> ! {
    loop {
        tgs_hal::gate::GATE.yield_now();
    }
}
