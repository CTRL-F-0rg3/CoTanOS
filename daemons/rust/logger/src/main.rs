#![no_std]
#![no_main]

#[panic_handler]
fn panic(_info: &core::panic::PanicInfo) -> ! {
    loop {}
}

const SYS_WRITE: usize = 64;
const SYS_EXIT: usize = 93;

fn syscall3(n: usize, a: usize, b: usize, c: usize) -> usize {
    let ret: usize;
    unsafe {
        core::arch::asm!(
            "ecall",
            in("a7") n,
            inlateout("a0") a => ret,
            in("a1") b,
            in("a2") c,
        );
    }
    ret
}

fn write_str(s: &str) {
    syscall3(SYS_WRITE, 1, s.as_ptr() as usize, s.len());
}

#[no_mangle]
pub extern "C" fn _start() -> ! {
    write_str("logger daemon started\n");
    loop {
        syscall3(SYS_EXIT, 0, 0, 0);
    }
}
