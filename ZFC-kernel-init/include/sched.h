#ifndef _SCHED_H
#define _SCHED_H

#include "queue.h"
#include "task.h"
#include <stdint.h>

/*
 * Stan schedulera. Jeden globalny obiekt w .bss (g_sched).
 * W trybie single-core boot hart jest jedynym, który go dotyka.
 */
struct scheduler {
  struct task *current; /* 0x00 – aktualnie wykonujące się zadanie */
  struct runqueue rq;   /* 0x08 – kolejka READY */
  struct task *idle;    /* 0x20 – zarezerwowane (przyszłość) */
  uint64_t boot_hart;   /* 0x28 – hart który przejął inicjalizację */
  uint64_t next_id;     /* 0x30 – następny wolny slot w g_tasks */
  uint64_t preempt_en;  /* 0x38 – czy sched_tick może wywłaszczać */
} __attribute__((packed));

#define SCHED_CURRENT 0x00
#define SCHED_RQ 0x08
#define SCHED_IDLE 0x20
#define SCHED_BOOT_HART 0x28
#define SCHED_NEXT_ID 0x30
#define SCHED_PREEMPT 0x38
#define SCHED_SIZE 0x40

/* Globalne symbole z sched.s */
extern struct scheduler g_sched;
extern struct task g_tasks[MAX_TASKS];

/* API (implementacja w sched.s) */
void sched_init(uint64_t hart_id);
struct task *sched_create(void (*entry)(void));
void sched_enqueue(struct task *t);
void sched_start(void) __attribute__((noreturn));
void sched_yield(void);
void sched_tick(void); /* z handlera timera */

/* Pomocnicze – eksponowane dla init.s */
void hart_park(void) __attribute__((noreturn));

#endif /* _SCHED_H */
