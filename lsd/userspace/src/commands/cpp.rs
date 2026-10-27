//! C++ applications, compiled from `apps/cpp` by build.rs and linked into the
//! kernel. Each exposes a freestanding `extern "C"` entry point that writes
//! NUL-terminated text into a caller-provided buffer.

use alloc::string::{String, ToString};
use alloc::vec;
use alloc::vec::Vec;

use super::{CmdResult, Ctx};

extern "C" {
    fn cpp_calc(args: *const u8, out: *mut u8, cap: usize) -> i32;
    fn cpp_temp(args: *const u8, out: *mut u8, cap: usize) -> i32;
    fn cpp_stats(args: *const u8, out: *mut u8, cap: usize) -> i32;
    fn cpp_bmi(args: *const u8, out: *mut u8, cap: usize) -> i32;
    fn cpp_rand(args: *const u8, out: *mut u8, cap: usize) -> i32;
    fn cpp_tui(args: *const u8, out: *mut u8, cap: usize) -> i32;
}

type CppFn = unsafe extern "C" fn(*const u8, *mut u8, usize) -> i32;

/// Run one C++ app: pass `args` as a NUL-terminated C string, receive its text
/// in a fixed buffer, and split the result into output lines.
fn run_cpp(f: CppFn, args: &str) -> CmdResult {
    let mut inbuf: Vec<u8> = Vec::new();
    inbuf.extend_from_slice(args.as_bytes());
    inbuf.push(0);

    let mut outbuf = vec![0u8; 4096];
    unsafe {
        f(inbuf.as_ptr(), outbuf.as_mut_ptr(), outbuf.len());
    }

    let end = outbuf.iter().position(|&b| b == 0).unwrap_or(outbuf.len());
    let text = String::from_utf8_lossy(&outbuf[..end]).into_owned();
    let lines: Vec<String> = text.lines().map(|s| s.to_string()).collect();
    CmdResult::ok(lines)
}

/// `calc` — C++ integer expression evaluator (`+ - * / % ( )`).
pub fn calc(_ctx: &mut Ctx<'_>, args: &str) -> CmdResult {
    run_cpp(cpp_calc, args)
}

/// `temp` — C++ temperature converter (C/F/K).
pub fn temp(_ctx: &mut Ctx<'_>, args: &str) -> CmdResult {
    run_cpp(cpp_temp, args)
}

/// `stats` — C++ min/max/sum/avg of numbers.
pub fn stats(_ctx: &mut Ctx<'_>, args: &str) -> CmdResult {
    run_cpp(cpp_stats, args)
}

/// `bmi` — C++ body mass index.
pub fn bmi(_ctx: &mut Ctx<'_>, args: &str) -> CmdResult {
    run_cpp(cpp_bmi, args)
}

/// `rand` — C++ pseudo-random numbers.
pub fn rand(_ctx: &mut Ctx<'_>, args: &str) -> CmdResult {
    run_cpp(cpp_rand, args)
}

/// `tui` — C++ ASCII dashboard (TUI-style frame).
pub fn tui(_ctx: &mut Ctx<'_>, args: &str) -> CmdResult {
    run_cpp(cpp_tui, args)
}
