/* SPDX-FileCopyrightText: 2026 CTRL-F-0rg3
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * CoTanSBI — own M-mode firmware for RISC-V (QEMU 'virt').
 * Loaded by QEMU as `-bios` at address 0x8000_0000.
 */

OUTPUT_ARCH(riscv)
ENTRY(_start)

BASE = 0x80000000;

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

    . = ALIGN(16);
    .bss (NOLOAD) : {
        __bss_start = .;
        *(.bss*) *(.sbss*)
        *(COMMON)
        . = ALIGN(16);
        __bss_end = .;
    }

    /* M-mode stack (used by the trap handler). */
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

/* The SBI must not reach the bootloader start (0x8020_0000). */
ASSERT(. < 0x80200000, "CoTanSBI: firmware too large (must fit below 0x80200000)");

