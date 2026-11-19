/* src/link.ld – linker script for kernel stage (init.s + sched.s + rest) */

OUTPUT_ARCH(riscv)
ENTRY(_start_kernel)

/* Base kernel address in physical memory.
   MUST match the address where boot.s does mret (mepc register). */
KERNEL_BASE = 0x80600000;

SECTIONS
{
    . = KERNEL_BASE;

    _kernel_start = .;

    /* ------------------------------------------------------------------
     * 1. Entry code – init.s MUST be first,
     *    so _start_kernel lands at a known, fixed address.
     *    (KEEP prevents section removal by --gc-sections)
     * ------------------------------------------------------------------ */
    .text.init : {
        KEEP(*(.text.init))
        KEEP(*(.text.startup))
    }

    /* ------------------------------------------------------------------
     * 2. Rest of the code – including sched.s (early scheduler),
     *    trap handlers, kernel C code, etc.
     *    If you want to isolate sched.s in a separate section, use
     *    .section .text.sched,"ax",@progbits in sched.s and add it here.
     * ------------------------------------------------------------------ */
    .text.sched : {
        *(.text.sched)
    }

    .text : {
        *(.text .text.*)
    }

    /* ------------------------------------------------------------------
     * 3. Read-only data (task descriptors, names, etc.)
     * ------------------------------------------------------------------ */
    .rodata : {
        *(.rodata .rodata.*)
        *(.srodata .srodata.*)
    }

    /* ------------------------------------------------------------------
     * 4. Initialized data (e.g., default values in task/queue structures,
     *    if non-zero – will land here from .data)
     * ------------------------------------------------------------------ */
    .data : {
        *(.data .data.*)
        *(.sdata .sdata.*)
    }

    /* ------------------------------------------------------------------
     * 5. BSS – structures from task.h / queue.h / sched.h will land here:
     *    - task array (e.g., struct task tasks[MAX_TASKS])
     *    - runqueue (struct runqueue rq)
     *    - scheduler global variables
     *    init.s will zero this area BEFORE jumping to C code.
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
     * 6. Kernel stack – init.s sets sp to its top before calling C.
     *    For early single-core scheduler, one stack is enough,
     *    but we also allocate "parking" stacks for other harts,
     *    so we can wake them up on wfi in the future.
     * ------------------------------------------------------------------ */
    .kernel_stack (NOLOAD) : {
        . = ALIGN(16);
        _kernel_stack = .;
        . += 0x4000;                /* 16 KiB – adjust as needed */
        _kernel_stack_top = .;
    }

    .hart_stacks (NOLOAD) : {
        . = ALIGN(16);
        __hart_stacks_start = .;
        . += 0x1000 * 8;            /* 4 KiB × 8 harts (reserve) */
        __hart_stacks_end = .;
    }

    /* ------------------------------------------------------------------
     * 7. Optional, isolated area for early scheduler data.
     *    Useful when you want them in one known place
     *    (e.g., for debugging or future SMP).
     *    If not used – you can remove this section.
     * ------------------------------------------------------------------ */
    .sched_data (NOLOAD) : {
        . = ALIGN(16);
        __sched_data_start = .;
        . += 0x2000;                /* space for task table / run queue */
        __sched_data_end = .;
    }

    _kernel_end = .;

    /* ------------------------------------------------------------------
     * 8. Junk – don't load them into memory
     * ------------------------------------------------------------------ */
    /DISCARD/ : {
        *(.comment)
        *(.note*)
        *(.eh_frame*)
        *(.riscv.attributes)
    }
}
