#ifndef _COTAN_GLOBAL_H
#define _COTAN_GLOBAL_H

#include "CoTan_types.h"

/* ------------------------------------------------------------------ */
/* Magic / version – szybka walidacja, czy g_cotan jest zainicjowany   */
/* ------------------------------------------------------------------ */
#define COTAN_MAGIC 0x436F54616E4F5331ULL   /* "CoTanOS1" */
#define COTAN_VERSION 0x0000000100000000ULL /* v1.0.0.0   */

/* ------------------------------------------------------------------ */
/* Fazy bootowania – zapisywane w g_cotan.phase na każdym etapie        */
/* ------------------------------------------------------------------ */
#define COTAN_PHASE_BOOT 0
#define COTAN_PHASE_PMM 1
#define COTAN_PHASE_VM 2
#define COTAN_PHASE_SCHED 3
#define COTAN_PHASE_KERNEL 4
#define COTAN_PHASE_RUNNING 5
#define COTAN_PHASE_PANIC 0xDEAD0000ULL

/* ------------------------------------------------------------------ */
/* Flagi systemowe                                                      */
/* ------------------------------------------------------------------ */
#define COTAN_FLAG_PANIC (1ULL << 0)
#define COTAN_FLAG_HALTING (1ULL << 1)
#define COTAN_FLAG_DEBUG (1ULL << 2)
#define COTAN_FLAG_SMP_READY (1ULL << 3)

#define COTAN_MAX_HARTS 8
#define COTAN_PANIC_MSG_LEN 64

/* ------------------------------------------------------------------ */
/* Per-hart – jeden wiersz cache (64 B). Dziś tylko hart 0 aktywny.     */
/* ------------------------------------------------------------------ */
struct cotan_per_hart {
  u64 hartid;              /* 0x00 */
  u64 online;              /* 0x08 */
  u64 current_task;        /* 0x10 */
  u64 sp_saved;            /* 0x18 */
  u64 ra_saved;            /* 0x20 */
  u64 trap_count;          /* 0x28 */
  u64 reserved[2];         /* 0x30 */
} __attribute__((packed)); /* 0x40 = 64 B */

/* ------------------------------------------------------------------ */
/* Główny blok – 512 B, 4 linie cache.                                 */
/* Układ: pola najczęściej używane w pierwszej linii (0x00–0x3F).       */
/* ------------------------------------------------------------------ */
struct cotan_global {
  /* ==== Linia 0: hot path (0x00–0x3F) ==== */
  u64 magic;        /* 0x00 */
  u64 version;      /* 0x08 */
  u64 phase;        /* 0x10 */
  u64 flags;        /* 0x18 */
  u64 boot_hartid;  /* 0x20 */
  u64 hart_count;   /* 0x28 */
  u64 current_task; /* 0x30 – mirror g_sched.current */
  u64 ticks;        /* 0x38 – inkrementowany w timerze */

  /* ==== Linia 1: mapa pamięci (0x40–0x7F) ==== */
  u64 dram_base;    /* 0x40 */
  u64 dram_size;    /* 0x48 */
  u64 kernel_start; /* 0x50 */
  u64 kernel_end;   /* 0x58 */
  u64 heap_start;   /* 0x60 */
  u64 heap_end;     /* 0x68 */
  u64 stack_start;  /* 0x70 */
  u64 stack_end;    /* 0x78 */

  /* ==== Linia 2: uchwyty podsystemów (0x80–0xBF) ==== */
  u64 sched_ptr;      /* 0x80 -> &g_sched */
  u64 pmm_bitmap_ptr; /* 0x88 -> &g_pmm_bitmap */
  u64 root_pt_ptr;    /* 0x90 -> &g_root_pt */
  u64 trap_vec_ptr;   /* 0x98 – aktualny stvec */
  u64 boot_dtb_pa;    /* 0xA0 – DTB fizycznie */
  u64 boot_ticks;     /* 0xA8 – snapshot time w momencie startu */
  u64 timer_freq;     /* 0xB0 – Hz timera */
  u64 log_level;      /* 0xB8 */

  /* ==== Linia 3: per-hart + statystyki (0xC0–0xFF) ==== */
  u64 hart_data_ptr;     /* 0xC0 -> &g_hart_data[0] */
  u64 boot_count;        /* 0xC8 */
  u64 total_irqs;        /* 0xD0 */
  u64 total_page_faults; /* 0xD8 */
  u64 total_syscalls;    /* 0xE0 */
  u64 last_trap_cause;   /* 0xE8 */
  u64 last_trap_epc;     /* 0xF0 */
  u64 lock;              /* 0xF8 – rezerwa pod przyszły spinlock SMP */

  /* ==== Panic (0x100–0x15F) ==== */
  u64 panic_hart;                      /* 0x100 */
  u64 panic_ra;                        /* 0x108 */
  u64 panic_cycle;                     /* 0x110 */
  u64 panic_phase;                     /* 0x118 */
  char panic_msg[COTAN_PANIC_MSG_LEN]; /* 0x120–0x15F */

  /* ==== Rezerwa (0x160–0x1FF) = 20 × u64 ==== */
  u64 reserved[20];        /* 0x160–0x1FF */
} __attribute__((packed)); /* razem 0x200 = 512 B */

/* ------------------------------------------------------------------ */
/* Offsety – dla .s i inline asm. Trzymać w synchronizacji ze strukturą. */
/* ------------------------------------------------------------------ */
#define COTAN_OFF_MAGIC 0x000
#define COTAN_OFF_VERSION 0x008
#define COTAN_OFF_PHASE 0x010
#define COTAN_OFF_FLAGS 0x018
#define COTAN_OFF_BOOT_HARTID 0x020
#define COTAN_OFF_HART_COUNT 0x028
#define COTAN_OFF_CURRENT_TASK 0x030
#define COTAN_OFF_TICKS 0x038
#define COTAN_OFF_DRAM_BASE 0x040
#define COTAN_OFF_DRAM_SIZE 0x048
#define COTAN_OFF_KERNEL_START 0x050
#define COTAN_OFF_KERNEL_END 0x058
#define COTAN_OFF_HEAP_START 0x060
#define COTAN_OFF_HEAP_END 0x068
#define COTAN_OFF_STACK_START 0x070
#define COTAN_OFF_STACK_END 0x078
#define COTAN_OFF_SCHED_PTR 0x080
#define COTAN_OFF_PMM_BITMAP 0x088
#define COTAN_OFF_ROOT_PT 0x090
#define COTAN_OFF_TRAP_VEC 0x098
#define COTAN_OFF_BOOT_DTB 0x0A0
#define COTAN_OFF_BOOT_TICKS 0x0A8
#define COTAN_OFF_TIMER_FREQ 0x0B0
#define COTAN_OFF_LOG_LEVEL 0x0B8
#define COTAN_OFF_HART_DATA 0x0C0
#define COTAN_OFF_BOOT_COUNT 0x0C8
#define COTAN_OFF_TOTAL_IRQS 0x0D0
#define COTAN_OFF_TOTAL_PF 0x0D8
#define COTAN_OFF_TOTAL_SYSCALL 0x0E0
#define COTAN_OFF_LAST_TRAP_CAUSE 0x0E8
#define COTAN_OFF_LAST_TRAP_EPC 0x0F0
#define COTAN_OFF_PANIC_HART 0x100
#define COTAN_OFF_PANIC_RA 0x108
#define COTAN_OFF_PANIC_CYCLE 0x110
#define COTAN_OFF_PANIC_PHASE 0x118
#define COTAN_OFF_PANIC_MSG 0x120

/* ------------------------------------------------------------------ */
/* Symbole globalne (z src/global.s)                                    */
/* ------------------------------------------------------------------ */
extern struct cotan_global g_cotan;
extern struct cotan_per_hart g_hart_data[COTAN_MAX_HARTS];

/* ------------------------------------------------------------------ */
/* Szybki dostęp – przez `tp`. WYMAGA wcześniejszego cotan_global_init. */
/* ------------------------------------------------------------------ */
static inline struct cotan_global *cotan_global(void) {
  struct cotan_global *p;
  __asm__ volatile("mv %0, tp" : "=r"(p));
  return p;
}

/* Najczęściej używane pola – inline, żeby kompilator nie robił load-barrier */
static inline u64 cotan_ticks(void) { return cotan_global()->ticks; }
static inline u64 cotan_phase(void) { return cotan_global()->phase; }
static inline struct task *cotan_current(void) {
  return (struct task *)cotan_global()->current_task;
}

static inline void cotan_set_phase(u64 p) { cotan_global()->phase = p; }

/* ------------------------------------------------------------------ */
/* API z src/global.s                                                   */
/* ------------------------------------------------------------------ */
/* a0 = hartid, a1 = dtb_pa (0 = brak) → 0 OK, -1 = nie-boot hart */
int cotan_global_init(u64 hart_id, u64 dtb_pa);

/* a0 = kod paniki, a1 = wskaźnik na wiadomość (może być NULL) – NIE WRACA */
void cotan_panic_halt(u64 code, const char *msg) __attribute__((noreturn));

#endif /* _COTAN_GLOBAL_H */
