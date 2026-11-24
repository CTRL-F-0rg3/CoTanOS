//! Obsługa wywołań systemowych (Linux RISC-V ABI)
use cotan_kapi::ffi::{uart_getc, uart_puts};
use log::info;

#[no_mangle]
pub extern "C" fn handle_syscall(nr: u64, a0: u64, a1: u64, a2: u64) -> i64 {
    match nr {
        64 => sys_write(a0 as i32, a1 as *const u8, a2 as usize),
        63 => sys_read(a0 as i32, a1 as *mut u8, a2 as usize),
        93 => sys_exit(a0 as i32),
        172 => sys_getpid(),
        _ => {
            info!("Unknown syscall: {}", nr);
            -38 // ENOSYS
        }
    }
}

fn sys_write(_fd: i32, buf: *const u8, len: usize) -> i64 {
    if len == 0 { return 0; }
    let slice = unsafe { core::slice::from_raw_parts(buf, len) };
    for &c in slice {
        // Naprawa: .as_ptr() zamienia &[u8; 2] w *const u8
        let echo_buf = [c, 0];
        unsafe { uart_puts(echo_buf.as_ptr()) };
    }
    len as i64
}

fn sys_read(fd: i32, buf: *mut u8, len: usize) -> i64 {
    if fd != 0 { return -9; } // EBADF (tylko stdin = 0)
    if len == 0 { return 0; }
    
    let slice = unsafe { core::slice::from_raw_parts_mut(buf, len) };
    let mut count = 0;
    
    for i in 0..len {
        let c = unsafe { uart_getc() };
        slice[i] = c;
        count += 1;
        
        // Echo znaku
        let echo_buf = [c, 0];
        unsafe { uart_puts(echo_buf.as_ptr()) };
        
        if c == b'\n' || c == b'\r' {
            break;
        }
    }
    count as i64
}

fn sys_exit(code: i32) -> i64 {
    info!("Process exited with code: {}", code);
    loop { unsafe { core::arch::asm!("wfi") } }
}

fn sys_getpid() -> i64 {
    1
}
