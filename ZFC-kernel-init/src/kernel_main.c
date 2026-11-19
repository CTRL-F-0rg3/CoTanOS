extern void start_user_process(void);
#include "CoTan_trap.h"
#include "CoTan_uart.h"
#include "CoTan_global.h"
#include "CoTan_mm.h"
#include "CoTan_pmm.h"
#include "CoTan_types.h"
#include "sched.h"

extern char _kernel_start[];
extern char _kernel_end[];

/* Symbole z mm.s / pmm.s – wystawiamy je w ich nagłówkach */
extern u64 g_pmm_base; /* już w CoTan_pmm.h */
extern u64 g_pmm_pages;
extern u64 g_pmm_free;
extern u64 g_root_pt;  /* from mm.s */


/* --- SMP Support --- */
static void wake_hart(int hartid) {
    uart_puts("[kernel] sending SBI IPI to hart ");
    // Prosty hack do wypisania cyfry (zakładamy hartid 1)
    uart_putc('0' + hartid);
    uart_puts("\n");
    
    register unsigned long a0 asm("a0") = (1UL << hartid); // hart_mask
    register unsigned long a1 asm("a1") = 0UL;             // hart_mask_base (MUST be 0 for single hart)
    register unsigned long a6 asm("a6") = 0x73444950UL;    // SBI_EXT_IPI
    register unsigned long a7 asm("a7") = 0UL;             // SBI_FN_IPI
    
    asm volatile(
        "ecall"
        : "+r"(a0), "+r"(a1)
        : "r"(a6), "r"(a7)
        : "memory"
    );
    uart_puts("[kernel] SBI IPI sent\n");
}

void secondary_hart_init(u64 hart_id) {
    trap_init(hart_id);
    uart_puts("[kernel] secondary hart ");
    uart_hex(hart_id);
    uart_puts(" awake, joining scheduler\n");
    
    // W xv6 sched_start obsługuje wiele hartów, jeśli są poprawnie zainicjalizowane.
    // Na razie po prostu wpadamy w pętlę schedulera.
    /* Obudź pozostałe harty (SMP) */
        extern volatile int g_init_done;
    g_init_done = 1;
    __sync_synchronize();

    uart_puts("[kernel] waking up secondary harts...\n");
    wake_hart(1); // Budzimy hart 1
    
    uart_puts("[kernel] starting user process...\n");
    start_user_process();
    
    uart_puts("[kernel] entering scheduler on hart 0\n");
    sched_start();
}


void task_a(void) {
    uart_puts("[task_a] started\n");
  for (;;)
    sched_yield();
}
void task_b(void) {
    uart_puts("[task_b] started\n");
  for (;;)
    sched_yield();
}

void kernel_main(u64 hart_id) {
    trap_init(hart_id);
    uart_puts("[kernel] entered kernel_main, hart="); uart_hex(hart_id); uart_putc(0x0a);
  struct cotan_global *G = cotan_global();
    uart_puts("[kernel] G="); uart_hex((u64)G); uart_putc(10);

  /* ----- Krok 1: statyczna mapa pamięci ----- */
  G->dram_base = DRAM_BASE;
  G->dram_size = DRAM_SIZE;
  G->kernel_start = (u64)_kernel_start;
  G->kernel_end = (u64)_kernel_end;
  G->boot_ticks = 0;        /* rdcycle tu jeszcze nie mierzone */
  G->timer_freq = 10000000; /* QEMU virt: 10 MHz timebase */

  /* ----- Krok 2: PMM ----- */
  G->phase = COTAN_PHASE_PMM;

  paddr_t heap_start = ((paddr_t)_kernel_end + PAGE_SIZE - 1) & PAGE_MASK;
  paddr_t heap_end = DRAM_BASE + DRAM_SIZE;

  uart_puts("[kernel] pmm_init\n");
    pmm_init(heap_start, heap_end - heap_start);
    uart_puts("[kernel] heap_start="); uart_hex((u64)heap_start);
    uart_puts(" heap_end="); uart_hex((u64)heap_end); uart_putc(10);
  pmm_mark_used(DRAM_BASE, heap_start - DRAM_BASE);

  G->heap_start = heap_start;
  G->heap_end = heap_end;
  G->pmm_bitmap_ptr = 0; /* uzupełnij, gdy dodasz eksport */

  /* ----- Krok 3: VM (Sv39, identity map) ----- */
  G->phase = COTAN_PHASE_VM;

  uart_puts("[kernel] mm_init\n");
    mm_init();
  uart_puts("[kernel] mm_map_kernel\n");
    mm_map_kernel();
  uart_puts("[kernel] mm_enable_paging\n");
    mm_enable_paging();
    uart_puts("[kernel] paging ON\n");

  G->root_pt_ptr = (u64)&g_root_pt; /* z mm.s – dodaj .globl */

  /* ----- Krok 4: Scheduler ----- */
  G->phase = COTAN_PHASE_SCHED;
  G->sched_ptr = (u64)&g_sched;

  sched_enqueue(sched_create(task_a));
  sched_enqueue(sched_create(task_b));

  /* ----- Krok 5: full speed ----- */
  G->hart_data_ptr = (u64)g_hart_data;
  G->phase = COTAN_PHASE_RUNNING;
  G->boot_count++;

  uart_puts("[kernel] sched_start\n");
    /* Obudź pozostałe harty (SMP) */
    uart_puts("[kernel] waking up secondary harts...\n");
    wake_hart(1); // Budzimy hart 1
    
    uart_puts("[kernel] starting user process...\n");
    start_user_process();
    
    uart_puts("[kernel] entering scheduler on hart 0\n");
    sched_start();
}
