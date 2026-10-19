/* TrangorgeOS Rust apps — bare ELF linker script (no C runtime, non-PIE).
 * Matches ustd/linker.ld: entry `_start`, static, loaded at the identity
 * address its PT_LOAD segments request. */
ENTRY(_start)

SECTIONS
{
    . = 0x00400000;
    _image_start = .;

    .text : ALIGN(4K)
    {
        *(.text.entry)
        *(.text .text.*)
    }

    . = ALIGN(4K);
    .rodata : ALIGN(4K)
    {
        *(.rodata .rodata.*)
    }

    .data : ALIGN(4K)
    {
        *(.data .data.*)
    }

    .bss (NOLOAD) : ALIGN(4K)
    {
        *(.bss .bss.*)
        *(COMMON)
    }

    . = ALIGN(4K);
    _image_end = .;

    /DISCARD/ :
    {
        *(.comment)
        *(.note.GNU-stack)
        *(.note.gnu.property)
        *(.eh_frame)
        *(.eh_frame_hdr)
    }
}
