/* SPDX-FileCopyrightText: 2026 CTRL-F-0rg3
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * CoTanOS 256xtr (arc-rv6) — RISC-V kernel linker script.
 * Loaded by QEMU `-kernel` and entered by the ini/ bootloader
 * at 0x8040_0000 (after CoTanSBI and the bootloader).
 */

OUTPUT_ARCH(riscv)
ENTRY(_start)

BASE = 0x80400000;

SECTIONS
{
    . = BASE;

    /* The entry code must come first so `_start` lands exactly on BASE. */
    .text : {
        KEEP(*(.text.entry))
        *(.text .text.*)
    }

    . = ALIGN(8);
    .rodata : { *(.rodata .rodata.*) }

    . = ALIGN(8);
    .data : { *(.data .data.*) *(.sdata .sdata.*) }

    . = ALIGN(4096);
    .bss (NOLOAD) : {
        __bss_start = .;
        *(.bss .bss.*) *(.sbss .sbss.*)
        *(COMMON)
        . = ALIGN(4096);
        __kernel_end = .;
    }

    /DISCARD/ : {
        *(.eh_frame)
        *(.comment)
        *(.note*)
    }
}

