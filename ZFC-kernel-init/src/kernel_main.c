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

void task_a(void) {
  for (;;)
    sched_yield();
}
void task_b(void) {
  for (;;)
    sched_yield();
}

void kernel_main(u64 hart_id) {
  struct cotan_global *G = cotan_global(); /* przez tp, zero kosztu */

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

  pmm_init(heap_start, heap_end - heap_start);
  pmm_mark_used(DRAM_BASE, heap_start - DRAM_BASE);

  G->heap_start = heap_start;
  G->heap_end = heap_end;
  G->pmm_bitmap_ptr = 0; /* uzupełnij, gdy dodasz eksport */

  /* ----- Krok 3: VM (Sv39, identity map) ----- */
  G->phase = COTAN_PHASE_VM;

  mm_init();
  mm_map_kernel();
  mm_enable_paging();

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

  sched_start(); /* nigdy nie wraca */
}
