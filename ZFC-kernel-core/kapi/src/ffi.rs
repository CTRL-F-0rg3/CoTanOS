extern "C" {
    pub fn uart_puts(s: *const u8);
    pub fn uart_hex(v: u64);
    pub fn uart_getc() -> u8; // <-- NOWE
    pub fn pmm_alloc_page() -> *mut u8;
    pub fn pmm_free_page(pa: *mut u8);
}
