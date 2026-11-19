/* SPDX-FileCopyrightText: 2026 CTRL-F-0rg3
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * CoTanBoot — bootloader S-mode (rola BIOS-u na x86_64).
 * Loaded by QEMU (`-device loader`) at 0x8020_0000.
 */

OUTPUT_ARCH(riscv)
ENTRY(_start)

BASE = 0x80200000;

SECTIONS
{
    . = BASE;

    /* The entry code (_start) must come first to land on BASE. */
    .text : {
        KEEP(*(.text.entry))
        *(.text*)
    }

    . = ALIGN(8);
    .rodata : { *(.rodata*) }

    . = ALIGN(8);
    .data : { *(.data*) *(.sdata*) }

    /*
     * BSS also holds the Sv39 page-table buffers (4 KiB root + 4 KiB L1),
     * zerowane przy starcie przez boot.S.
     */
    . = ALIGN(4096);
    .bss (NOLOAD) : {
        __bss_start = .;
        *(.bss*) *(.sbss*)
        *(COMMON)
        . = ALIGN(4096);
        __pg_root = .;
        . += 8192;          /* 2 × 4 KiB: root (L2) + L1 dla RAM-u */
        . = ALIGN(16);
        __bss_end = .;
    }

    /* Stos trybu S bootloadera. */
    . = ALIGN(16);
    .stack (NOLOAD) : {
        . += 0x10000;
        __stack_top = .;
    }

    /DISCARD/ : {
        *(.eh_frame)
        *(.comment)
        *(.note*)
    }
}

/* The bootloader must not reach the kernel area (0x8040_0000). */
ASSERT(. < 0x80400000, "CoTanBoot: bootloader too large (must fit below 0x80400000)");

