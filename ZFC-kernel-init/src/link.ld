/* src/link.ld – linker script dla etapu jądra (init.s + sched.s + reszta) */

OUTPUT_ARCH(riscv)
ENTRY(_start_kernel)

/* Adres bazowy jądra w pamięci fizycznej.
   MUSI być zgodny z adresem, pod który boot.s robi mret (rejestr mepc). */
KERNEL_BASE = 0x80200000;

SECTIONS
{
    . = KERNEL_BASE;

    _kernel_start = .;

    /* ------------------------------------------------------------------
     * 1. Kod wejściowy jądra – init.s MUSI być pierwszy,
     *    żeby _start_kernel trafił pod znany, ustalony adres.
     *    (KEEP zapobiega usunięciu sekcji przez --gc-sections)
     * ------------------------------------------------------------------ */
    .text.init : {
        KEEP(*(.text.init))
        KEEP(*(.text.startup))
    }

    /* ------------------------------------------------------------------
     * 2. Reszta kodu – w tym sched.s (wczesny scheduler),
     *    handlery trapów, kod C jądra itd.
     *    Jeśli chcesz wyróżnić sched.s osobną sekcją, w sched.s użyj
     *    .section .text.sched,"ax",@progbits i dodaj ją tutaj.
     * ------------------------------------------------------------------ */
    .text.sched : {
        *(.text.sched)
    }

    .text : {
        *(.text .text.*)
    }

    /* ------------------------------------------------------------------
     * 3. Dane tylko do odczytu (tablice opisów zadań, nazwy itd.)
     * ------------------------------------------------------------------ */
    .rodata : {
        *(.rodata .rodata.*)
        *(.srodata .srodata.*)
    }

    /* ------------------------------------------------------------------
     * 4. Dane zainicjowane (np. domyślne wartości w strukturach task/queue,
     *    jeśli nie są zerowe – trafią tu z .data)
     * ------------------------------------------------------------------ */
    .data : {
        *(.data .data.*)
        *(.sdata .sdata.*)
    }

    /* ------------------------------------------------------------------
     * 5. BSS – tu wylądują struktury zdefiniowane w task.h / queue.h / sched.h:
     *    - tablica tasków (np. struct task tasks[MAX_TASKS])
     *    - kolejka runqueue (struct runqueue rq)
     *    - zmienne globalne schedulera
     *    init.s wyzeruje ten obszar PRZED skokiem do kodu C.
     * ------------------------------------------------------------------ */
    .bss (NOLOAD) : {
        . = ALIGN(16);
        __bss_start = .;
        *(.bss .bss.*)
        *(.sbss .sbss.*)
        *(COMMON)
        . = ALIGN(16);
        __bss_end = .;
    }

    /* ------------------------------------------------------------------
     * 6. Stos jądra – init.s ustawia na jego szczyt sp przed call do C.
     *    Dla wczesnego, jednordzeniowego schedulera wystarczy jeden stos,
     *    ale alokujemy też "parkingowe" stosy dla pozostałych hartów,
     *    żeby w przyszłości móc je uśpić na wfi.
     * ------------------------------------------------------------------ */
    .kernel_stack (NOLOAD) : {
        . = ALIGN(16);
        _kernel_stack = .;
        . += 0x4000;                /* 16 KiB – dostosuj do potrzeb */
        _kernel_stack_top = .;
    }

    .hart_stacks (NOLOAD) : {
        . = ALIGN(16);
        __hart_stacks_start = .;
        . += 0x1000 * 8;            /* 4 KiB × 8 hartów (zapas) */
        __hart_stacks_end = .;
    }

    /* ------------------------------------------------------------------
     * 7. Opcjonalny, wydzielony obszar na dane wczesnego schedulera.
     *    Przydatne, gdy chcesz mieć je w jednym, znanym miejscu
     *    (np. do debugowania albo pod przyszłe SMP).
     *    Jeśli nie używasz – możesz usunąć tę sekcję.
     * ------------------------------------------------------------------ */
    .sched_data (NOLOAD) : {
        . = ALIGN(16);
        __sched_data_start = .;
        . += 0x2000;                /* miejsce na task table / run queue */
        __sched_data_end = .;
    }

    _kernel_end = .;

    /* ------------------------------------------------------------------
     * 8. Śmieci – nie ładuj ich do pamięci
     * ------------------------------------------------------------------ */
    /DISCARD/ : {
        *(.comment)
        *(.note*)
        *(.eh_frame*)
        *(.riscv.attributes)
    }
}
