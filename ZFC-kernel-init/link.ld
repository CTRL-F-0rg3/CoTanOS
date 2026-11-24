/*
 * CoTanOS linker script - S-mode @ 0x80600000 (OpenSBI fw_dynamic).
 * _kernel_end MUST be after all stacks: kernel_main derives the PMM
 * heap base from it, and a heap overlapping the stacks corrupts frames.
 * CoTanOS GPLv3
 */
OUTPUT_ARCH("riscv")
ENTRY(_start_kernel)

SECTIONS
{
    . = 0x80600000;
    _kernel_start = .;

    .text : {
        KEEP(*(.text.init))
        *(.text .text.*)
    }

    .rodata : ALIGN(8) { *(.rodata .rodata.*) }
    .data   : ALIGN(8) { *(.data .data.*) }

    .bss : ALIGN(8) {
        __bss_start = .;
        *(.bss .bss.*)
        *(COMMON)
        . = ALIGN(8);
        __bss_end = .;
    }

    /* Stosy: NOLOAD, nie zaśmiecają obrazu ładowanego przez OpenSBI */
    .stack ALIGN(4096) (NOLOAD) : {
        _kernel_stack_bottom = .;
        . += 16 * 1024;
        _kernel_stack_top = .;
        _stack_top = .;                 /* alias dla starszego kodu */

        . = ALIGN(4096);
        __hart_stacks_start = .;
        . += 8 * 4096;
        __hart_stacks_end = .;
    }

    . = ALIGN(4096);
    _kernel_end = .;                    /* <-- dopiero tutaj */

    /DISCARD/ : {
        *(.comment)
        *(.note .note.*)
        *(.riscv.attributes)
        *(.eh_frame*)
    }
}
