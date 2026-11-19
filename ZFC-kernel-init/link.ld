/* 
 * CoTanOS Linker Script (RISC-V 64-bit)
 * Target: OpenSBI / QEMU (S-mode @ 0x80200000)
 */

OUTPUT_ARCH("riscv")
ENTRY(_start)

SECTIONS
{
    /* Początek pamięci dla S-mode w OpenSBI */
    . = 0x80200000;

    /* Sekcja rozruchowa i kod wykonywalny */
    .text : {
        KEEP(*(.text.init))
        *(.text .text.*)
    }

    /* Dane tylko do odczytu */
    .rodata : {
        . = ALIGN(8);
        *(.rodata .rodata.*)
    }

    /* Zainicjalizowane dane globalne */
    .data : {
        . = ALIGN(8);
        *(.data .data.*)
    }

    /* Niezainicjalizowane dane (.bss) - wymagają wyczyszczenia przy starcie */
    .bss : {
        . = ALIGN(8);
        __bss_start = .;
        *(.bss .bss.*)
        *(COMMON)
        . = ALIGN(8);
        __bss_end = .;
    }

    /* Alokacja stosu kernela (np. 16 KB na początek) */
    . = ALIGN(16);
    . += 16 * 1024;
    _stack_top = .;

    /DISCARD/ : {
        *(.comment)
        *(.note .note.*)
    }
}
