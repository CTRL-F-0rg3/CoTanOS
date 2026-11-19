#ifndef _TASK_H
#define _TASK_H

#include <stdint.h>

/* Stany zadania */
#define TASK_UNUSED 0
#define TASK_READY 1
#define TASK_RUNNING 2
#define TASK_BLOCKED 3
#define TASK_DONE 4

#define MAX_TASKS 16
#define TASK_STACK_SIZE 0x1000 /* 4 KiB na zadanie */

/*
 * Struktura zadania. Układ pól MUSI zgadzać się z offsetami
 * w sched.s (patrz .equ TASK_* na górze pliku).
 * __attribute__((packed)) -> brak paddingu, przewidywalne offsety.
 */
struct task {
  /* kontekst sprzętowy – zapisywany przez context_switch */
  uint64_t ra;  /* 0x00 */
  uint64_t sp;  /* 0x08 */
  uint64_t s0;  /* 0x10 */
  uint64_t s1;  /* 0x18 */
  uint64_t s2;  /* 0x20 */
  uint64_t s3;  /* 0x28 */
  uint64_t s4;  /* 0x30 */
  uint64_t s5;  /* 0x38 */
  uint64_t s6;  /* 0x40 */
  uint64_t s7;  /* 0x48 */
  uint64_t s8;  /* 0x50 */
  uint64_t s9;  /* 0x58 */
  uint64_t s10; /* 0x60 */
  uint64_t s11; /* 0x68 */

  /* metadane */
  uint64_t id;         /* 0x70 */
  uint64_t state;      /* 0x78 */
  void (*entry)(void); /* 0x80 */
  uint64_t stack_base; /* 0x88 */
  uint64_t ticks;      /* 0x90 */
  struct task *next;   /* 0x98 – link w runqueue */
} __attribute__((packed));

/* Offsety dla asm – trzymać w synchronizacji z powyższą strukturą */
#define TASK_RA 0x00
#define TASK_SP 0x08
#define TASK_S0 0x10
#define TASK_S1 0x18
#define TASK_S2 0x20
#define TASK_S3 0x28
#define TASK_S4 0x30
#define TASK_S5 0x38
#define TASK_S6 0x40
#define TASK_S7 0x48
#define TASK_S8 0x50
#define TASK_S9 0x58
#define TASK_S10 0x60
#define TASK_S11 0x68
#define TASK_ID 0x70
#define TASK_STATE 0x78
#define TASK_ENTRY 0x80
#define TASK_STACK 0x88
#define TASK_TICKS 0x90
#define TASK_NEXT 0x98
#define TASK_SIZE 0xA0

#endif /* _TASK_H */
