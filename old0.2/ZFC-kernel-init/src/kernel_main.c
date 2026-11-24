/*
 * CoTanOS Kernel Main Entry Point
 */

#include "CoTan_types.h"
#include "CoTan_pmm.h"
#include "CoTan_mm.h"
#include "CoTan_trap.h"
#include "CoTan_uart.h"
#include "CoTan_global.h"
#include "sched.h"

extern int core_init(u64 dtb_addr);

// External symbols from linker script
extern char _kernel_start[];
extern char _kernel_end[];

// VGA display functions
extern int qemu_vga_init(u32 width, u32 height);
extern void qemu_vga_draw_test_pattern(void);

void secondary_hart_init(u64 hart_id) {
    uart_puts("[kernel] secondary hart ");
    uart_hex(hart_id);
    uart_puts(" awake, parking\n");
    while (1) {
        __asm__ volatile ("wfi");
    }
}

void kernel_main(u64 hartid, u64 dtb) {
    uart_puts("[kernel] entered kernel_main, hart=");
    uart_hex(hartid);
    uart_puts("\n");

    uart_puts("[kernel] G=");
    uart_hex((u64)&g_cotan);
    uart_puts("\n");

    uart_puts("[kernel] copying DTB to safe kernel memory...\n");
    // FDT header: magic(4) totalsize(4) off_dt_struct(4) off_dt_strings(4) ...
    // totalsize is at offset 0x04, BIG-ENDIAN
    u32 *fdt_hdr = (u32 *)dtb;
    u32 fdt_totalsize_be = fdt_hdr[1];
    u32 fdt_totalsize = __builtin_bswap32(fdt_totalsize_be);
    uart_puts("[kernel] DTB totalsize = ");
    uart_hex(fdt_totalsize);
    uart_puts(" bytes\n");

    // Sanity check: FDT magic must be 0xd00dfeed (big-endian)
    if (__builtin_bswap32(fdt_hdr[0]) != 0xd00dfeed) {
        uart_puts("[kernel] FATAL: bad FDT magic!\n");
        while(1) __asm__ volatile("wfi");
    }

    // Allocate safe buffer: use a dedicated region far from g_cotan
    // 64KB should be plenty for QEMU virt DTB (typically ~30KB)
    u64 *safe_dtb = (u64 *)0x80800000;  // above kernel, below framebuffer
    u64 *src = (u64 *)dtb;
    u32 words = (fdt_totalsize + 7) / 8;
    for (u32 i = 0; i < words; i++) {
        safe_dtb[i] = src[i];
    }
    
    uart_puts("[kernel] DTB safely copied to ");
    uart_hex((u64)safe_dtb);
    uart_puts("\n");

    // Initialize PMM with memory after kernel
    paddr_t pmm_base = (paddr_t)_kernel_end;
    u64 pmm_size = 128 * 1024 * 1024; // 128MB for QEMU
    
    uart_puts("[kernel] pmm_init\n");
    pmm_init(pmm_base, pmm_size);
    
    uart_puts("[kernel] mm_init\n");
    mm_init();
    
    uart_puts("[kernel] mm_map_kernel\n");
    mm_map_kernel();
    
    uart_puts("[kernel] mm_enable_paging\n");
    mm_enable_paging();
    uart_puts("[kernel] paging ON\n");

    uart_puts("[kernel] calling Rust core_init...\n");
    int rust_result = core_init((u64)safe_dtb);
    uart_puts("[kernel] Rust core returned: ");
    uart_hex((u64)rust_result);
    uart_puts("\n");

    // Initialize Display Subsystem
    uart_puts("[kernel] initializing display subsystem...\n");
    if (qemu_vga_init(1024, 768) == 0) {
        qemu_vga_draw_test_pattern();
        uart_puts("[kernel] display test pattern rendered.\n");
    } else {
        uart_puts("[kernel] display initialization skipped/failed.\n");
    }

    uart_puts("[kernel] sched_start\n");
    sched_start();
    
    while (1) {
        __asm__ volatile ("wfi");
    }
}
