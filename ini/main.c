/* SPDX-FileCopyrightText: 2026 CTRL-F-0rg3
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * ini/ — CoTanOS Hardware Bootloader (S-mode).
 * BIOS role on x86_64: memory discovery (FDT), Sv39 paging with a
 * 1:1 map (physical memory + MMIO), welcome screen (ramfb) and the
 * hand-off to the kernel.
 */

#include <stdint.h>
#include "sbi.h"
#include "fdt.h"
#include "bootinfo.h"
#include "gfx.h"
#include "font_atlas.h"

#define KERNEL_ENTRY 0x80400000UL

/* --- Sv39 --- */
#define SATP_MODE_SV39 (8UL << 60)

#define PTE_V (1UL << 0)
#define PTE_R (1UL << 1)
#define PTE_W (1UL << 2)
#define PTE_X (1UL << 3)
#define PTE_A (1UL << 6)
#define PTE_D (1UL << 7)

/* All permissions + A/D set upfront (no A/D traps). */
#define PTE_FLAGS (PTE_V | PTE_R | PTE_W | PTE_X | PTE_A | PTE_D)

#define PAGE_ENTRIES 512

/* Page-table buffers (zeroed .bss, 4 KiB aligned). */
extern char __pg_root[];

/* Bootinfo for the kernel (in .data). */
static struct bootinfo bootinfo = {
    .magic = BOOTINFO_MAGIC,
    .version = BOOTINFO_VERSION,
    .mem_base = 0,
    .mem_size = 0,
    .fdt_addr = 0,
    .kernel_entry = KERNEL_ENTRY,
    .fb_addr = 0,
    .fb_width = 0,
    .fb_height = 0,
    .fb_bpp = 0,
    .fb_pitch = 0,
};

static void puts(const char *s)
{
    while (*s) {
        sbi_console_putchar(*s++);
    }
}

void boot_fatal_trap(uint64_t scause, uint64_t sepc)
    __attribute__((noreturn));

void boot_fatal_trap(uint64_t scause, uint64_t sepc)
{
    puts("\r\n[CoTanBoot] FATAL S-mode trap! scause=");
    /* hex, no libc */
    for (int i = 60; i >= 0; i -= 4) {
        char d = "0123456789abcdef"[(scause >> i) & 0xF];
        sbi_console_putchar(d);
    }
    puts(" sepc=");
    for (int i = 60; i >= 0; i -= 4) {
        char d = "0123456789abcdef"[(sepc >> i) & 0xF];
        sbi_console_putchar(d);
    }
    puts("\r\n[CoTanBoot] halting.\r\n");
    for (;;) {
        __asm__ volatile ("wfi");
    }
    return 0xFFFFFFFFu;
}

/*
 * Sv39 page-table build (3 levels, 1:1 mapping):
 *  - root (L2): 512 entries x 1 GiB
 *      [0] = gigapage 0x0000_0000..0x4000_0000 (MMIO: UART, CLINT, PLIC, ...)
 *      [1] = gigapage 0x4000_0000..0x8000_0000 (PCIe MMIO)
 *      [2] = L1 table for 0x8000_0000..0xC000_0000 (RAM)
 *  - L1: 2 MiB megapages — the whole RAM gigapage.
 */
static void build_sv39_identity(uint64_t root_phys)
{
    uint64_t *root = (uint64_t *)root_phys;
    uint64_t *l1 = (uint64_t *)(root_phys + 4096);

    for (int i = 0; i < PAGE_ENTRIES; i++) {
        root[i] = 0;
        l1[i] = 0;
    }

    /* MMIO gigapage: physical address 0, ppn = 0. */
    root[0] = PTE_FLAGS;

    /* PCIe MMIO gigapage (0x4000_0000..0x8000_0000): VGA, virtio, ... */
    root[1] = ((0x40000000UL >> 12) << 10) | PTE_FLAGS;

    /* RAM: 512 2 MiB megapages from 0x8000_0000. */
    for (int i = 0; i < PAGE_ENTRIES; i++) {
        uint64_t pa = 0x80000000UL + (uint64_t)i * 0x200000UL;
        l1[i] = ((pa >> 12) << 10) | PTE_FLAGS;
    }
    root[2] = (((root_phys + 4096) >> 12) << 10) | PTE_V;
}

static void enable_paging(uint64_t root_phys)
{
    uint64_t satp = SATP_MODE_SV39 | (root_phys >> 12);
    __asm__ volatile ("csrw satp, %0" : : "r"(satp));
    /* Invalidate the TLB — virtual addresses == physical. */
    __asm__ volatile ("sfence.vma zero, zero" ::: "memory");
}

/* Decimal number -> buffer (no libc). */
static void fmt_dec(char *out, uint64_t v)
{
    char tmp[24];
    int i = 0;
    if (v == 0) {
        tmp[i++] = '0';
    }
    while (v > 0) {
        tmp[i++] = (char)('0' + (v % 10));
        v /= 10;
    }
    while (i > 0) {
        *out++ = tmp[--i];
    }
    *out = '\0';
}

typedef void (*kentry_t)(uint64_t hartid, uint64_t fdt,
                         struct bootinfo *info);

/* Hex number -> buffer (no libc). */
static void fmt_hex(char *out, uint64_t v)
{
    static const char d[] = "0123456789ABCDEF";
    out[0] = '0';
    out[1] = 'X';
    for (int i = 0; i < 16; i++) {
        out[2 + i] = d[(v >> ((15 - i) * 4)) & 0xF];
    }
    out[18] = '\0';
}

/*
 * Boot screen in the macOS style:
 *   black background + white "COTANOS" centered.
 * The kernel keeps this screen and adds the spinning star outline
 * at the bottom until userspace takes over.
 */
static void draw_boot_screen(void)
{
    if (gfx_init() != 0) {
        puts("[CoTanBoot] no ramfb — screen skipped.\r\n");
        return;
    }

    bootinfo.fb_addr   = gfx_fb_addr();
    bootinfo.fb_width  = GFX_WIDTH;
    bootinfo.fb_height = GFX_HEIGHT;
    bootinfo.fb_bpp    = GFX_BPP;
    bootinfo.fb_pitch  = GFX_WIDTH * (GFX_BPP / 8);

    /* Clean, black screen. */
    gfx_fill(GFX_COL_BG);

    /* White "COTANOS" centered (DejaVu Sans 19 px, anti-aliased). */
    const char *brand = "COTANOS";
    gfx_text_aa((GFX_WIDTH - gfx_text_width_aa(brand, &FONT_19)) / 2,
                GFX_HEIGHT / 2 - 10, brand, GFX_COL_WHITE, &FONT_19);

    puts("[CoTanBoot] boot screen drawn (ramfb 640x480x32) @ ");
    {
        char buf[24];
        fmt_hex(buf, bootinfo.fb_addr);
        puts(buf);
    }
    puts("\r\n");
}

void init_main(uint64_t hartid, uint64_t fdt_hint)
{
    const void *fdt = fdt_locate(fdt_hint);
    uint64_t mem_base = 0x80000000UL;
    uint64_t mem_size = 0x08000000UL; /* default 128 MiB (QEMU virt) */

    puts("\r\n[CoTanBoot] S-mode bootloader (BIOS role) — hart ");
    /* simple decimal number, no libc */
    {
        uint64_t v = hartid;
        char buf[24];
        int i = 0;
        if (v == 0) {
            sbi_console_putchar('0');
        }
        while (v > 0) {
            buf[i++] = (char)('0' + (v % 10));
            v /= 10;
        }
        while (i > 0) {
            sbi_console_putchar(buf[--i]);
        }
    }
    puts("\r\n");

    if (fdt) {
        if (fdt_memory(fdt, &mem_base, &mem_size) != 0) {
            puts("[CoTanBoot] warning: FDT has no /memory node — assuming "
                 "default 128 MiB.\r\n");
        }
        bootinfo.fdt_addr = (uint64_t)fdt;
    } else {
        puts("[CoTanBoot] warning: FDT not found — assuming default "
             "128 MiB.\r\n");
    }
    bootinfo.mem_base = mem_base;
    bootinfo.mem_size = mem_size;

    puts("[CoTanBoot] RAM: base=");
    for (int i = 60; i >= 0; i -= 4) {
        char d = "0123456789abcdef"[(mem_base >> i) & 0xF];
        sbi_console_putchar(d);
    }
    puts(" size=");
    for (int i = 60; i >= 0; i -= 4) {
        char d = "0123456789abcdef"[(mem_size >> i) & 0xF];
        sbi_console_putchar(d);
    }
    puts("\r\n");

    /* Physical 1:1 mapping (Sv39) — the BIOS job on x86_64. */
    build_sv39_identity((uint64_t)(uintptr_t)__pg_root);
    enable_paging((uint64_t)(uintptr_t)__pg_root);
    puts("[CoTanBoot] Sv39 paging active (1:1 map: RAM + MMIO).\r\n");

    /* The boot screen (black + white CoTanOS); the kernel adds the spinner. */
    draw_boot_screen();

    /* S-mode timer (CLINT) via SBI — STIE in sie, SIE in sstatus. */
    __asm__ volatile ("csrs sie, %0" : : "r"(1UL << 5));
    __asm__ volatile ("csrs sstatus, %0" : : "r"(1UL << 1));

    puts("[CoTanBoot] handing off to 256xtr kernel @ ");
    for (int i = 60; i >= 0; i -= 4) {
        char d = "0123456789abcdef"[((uint64_t)KERNEL_ENTRY >> i) & 0xF];
        sbi_console_putchar(d);
    }
    puts("...\r\n");

    kentry_t kernel = (kentry_t)KERNEL_ENTRY;
    kernel(hartid, (uint64_t)fdt, &bootinfo);

    /* The kernel must not return. */
    puts("[CoTanBoot] kernel returned — halting.\r\n");
    for (;;) {
        __asm__ volatile ("wfi");
    }
}
