use core::panic::PanicInfo;
use cotan_kapi::kprintln;

#[panic_handler]
fn panic(info: &PanicInfo) -> ! {
    kprintln!("[rust PANIC] {}", info);
    loop {
        unsafe { core::arch::asm!("wfi") };
    }
}
