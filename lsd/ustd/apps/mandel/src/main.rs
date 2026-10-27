//! `mandel` — an ASCII Mandelbrot renderer written with the *full* Rust `std`.
//!
//! No `no_std`, no `unsafe`: an ordinary Rust program (`f64`, `String`,
//! `Instant`, `print!`) compiled against the TrangorgeOS sysroot and run in
//! ring 3. This is the "more advanced" counterpart to the freestanding
//! `apps/rust/hello`.

// Link the `_start` entry point and `main` shim (not `use`d below).
extern crate tgs_rt;

use std::time::Instant;

const WIDTH: usize = 80;
const HEIGHT: usize = 40;
const ITER: u32 = 64;
const RAMP: &[u8] = b" .:-=+*#%@";

fn main() {
    println!("Mandelbrot set (ASCII {WIDTH}x{HEIGHT}, {ITER} iterations)");
    let t0 = Instant::now();

    let mut frame = String::with_capacity(WIDTH * HEIGHT + HEIGHT);
    for py in 0..HEIGHT {
        let y = -1.0 + 2.0 * (py as f64) / (HEIGHT as f64);
        for px in 0..WIDTH {
            let x = -2.5 + 3.5 * (px as f64) / (WIDTH as f64);
            frame.push(shade(mandelbrot(x, y)));
        }
        frame.push('\n');
    }

    print!("{frame}");
    println!("rendered in {:?}", t0.elapsed());
}

fn mandelbrot(x0: f64, y0: f64) -> u32 {
    let (mut x, mut y) = (0.0f64, 0.0f64);
    let mut it = 0;
    while it < ITER && x * x + y * y <= 4.0 {
        let xt = x * x - y * y + x0;
        y = 2.0 * x * y + y0;
        x = xt;
        it += 1;
    }
    it
}

fn shade(it: u32) -> char {
    let idx = (it as usize).saturating_mul(RAMP.len() - 1) / (ITER as usize);
    RAMP[idx] as char
}
