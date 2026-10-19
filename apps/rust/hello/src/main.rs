//! hello — the smallest freestanding TrangorgeOS Rust app: print and exit.
//!
//! Uses the same ABI as `apps/zig/rt.zig`: `int 0x80` with `rax` = syscall
//! number, `rdi` = first argument. `LOG` (2) prints a NUL-terminated string on
//! the kernel console, `TGS_EXIT` (0x1000) terminates the world.

#![no_std]
#![no_main]

use core::arch::asm;
use core::panic::PanicInfo;

const LOG: u64 = 2;
const TGS_EXIT: u64 = 0x1000;

#[inline]
unsafe fn syscall1(num: u64, a0: u64) -> u64 {
    let ret: u64;
    asm!(
        "int 0x80",
        inlateout("rax") num => ret,
        in("rdi") a0,
        options(nostack)
    );
    ret
}

#[no_mangle]
pub extern "C" fn _start() -> ! {
    let msg = b"hello from Rust on TrangorgeOS\0";
    unsafe {
        syscall1(LOG, msg.as_ptr() as u64);
        syscall1(TGS_EXIT, 0);
    }
    loop {}
}

#[panic_handler]
fn panic(_info: &PanicInfo) -> ! {
    loop {}
}
