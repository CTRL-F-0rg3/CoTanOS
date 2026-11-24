//! Konsola kernela: buforowane ramki NUL-terminated wołające uart_puts.
use core::fmt::{self, Write};
use crate::ffi::uart_puts;

pub struct Uart;

impl Write for Uart {
    fn write_str(&mut self, s: &str) -> fmt::Result {
        let mut buf = [0u8; 256];
        for chunk in s.as_bytes().chunks(buf.len() - 1) {
            buf[..chunk.len()].copy_from_slice(chunk);
            buf[chunk.len()] = 0;
            unsafe { uart_puts(buf.as_ptr()) };
        }
        Ok(())
    }
}

pub fn _print(args: fmt::Arguments) {
    let _ = Uart.write_fmt(args);
}

#[macro_export]
macro_rules! kprint {
    ($($a:tt)*) => { $crate::console::_print(format_args!($($a)*)) };
}

#[macro_export]
macro_rules! kprintln {
    () => { $crate::kprint!("\n") };
    ($($a:tt)*) => { $crate::kprint!("{}\n", format_args!($($a)*)) };
}
