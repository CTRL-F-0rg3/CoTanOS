//! Interfejs FFI do C init

// Wywołania do C init
extern "C" {
    // Alokuj stronę pamięci (z C PMM)
    pub fn pmm_alloc_page() -> *mut u8;
    
    // Zwolnij stronę pamięci
    pub fn pmm_free_page(page: *mut u8);
    
    // Wypisz string do UART
    pub fn uart_puts(s: *const u8);
    
    // Wypisz hex do UART
    pub fn uart_hex(val: u64);
}

/// Wrapper dla uart_puts
pub fn kernel_print(s: &str) {
    unsafe {
        uart_puts(s.as_ptr());
    }
}

/// Wrapper dla uart_hex
pub fn kernel_print_hex(val: u64) {
    unsafe {
        uart_hex(val);
    }
}
