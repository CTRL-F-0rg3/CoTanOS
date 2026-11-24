OUTPUT_ARCH(riscv)
ENTRY(_start)

SECTIONS
{
    . = 0x80640000;  /* Po kodzie C init */
    
    .text : {
        *(.text .text.*)
    }
    
    .rodata : ALIGN(4) {
        *(.rodata .rodata.*)
    }
    
    .data : ALIGN(4) {
        *(.data .data.*)
    }
    
    .bss : ALIGN(8) {
        *(.bss .bss.*)
        *(COMMON)
    }
    
    /DISCARD/ : {
        *(.comment)
        *(.eh_frame)
    }
}
