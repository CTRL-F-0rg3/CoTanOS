//! Obsługa paniki w kernelu
use core::panic::PanicInfo;

#[panic_handler]
fn panic(info: &PanicInfo) -> ! {
    // W przyszłości tutaj wywołamy uart_puts z komunikatem błędu
    // Na razie bezpieczne zawieszenie hartu
    loop {
        core::arch::asm!("wfi");
    }
}
