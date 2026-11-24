OUTPUT_ARCH(riscv)
ENTRY(_start)
SECTIONS {
    . = 0x40000000;
    .text : {
        *(.text.init)
        *(.text*)
    }
    .rodata : ALIGN(4) {
        *(.rodata*)
        *(.srodata*)
    }
    .data : ALIGN(4096) {
        *(.data*)
        *(.sdata*)
        PROVIDE(__global_pointer$ = . + 0x800);
    }
    .bss : ALIGN(4) {
        *(.sbss*)
        *(.bss*)
        *(COMMON)
    }
    /DISCARD/ : {
        *(.comment)
        *(.riscv.attributes)
        *(.eh_frame*)
    }
}
