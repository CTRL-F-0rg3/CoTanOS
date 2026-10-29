ENTRY(_start)

SECTIONS
{
    /* W architekturze 'virt' QEMU ładowany kod trafia domyślnie pod 0x80000000 */
    . = 0x80000000;

    .text : {
        *(.text._start)
        *(.text)
    }

    .data : {
        *(.data)
    }

    .bss : {
        *(.bss)
    }
}