#ifndef _QUEUE_H
#define _QUEUE_H

#include "task.h"
#include <stdint.h>

/*
 * Prosta kolejka FIFO zadań (runqueue).
 * Linkowanie przez pole task->next.
 */
struct runqueue {
  struct task *head; /* 0x00 */
  struct task *tail; /* 0x08 */
  uint64_t count;    /* 0x10 */
} __attribute__((packed));

#define RQ_HEAD 0x00
#define RQ_TAIL 0x08
#define RQ_COUNT 0x10
#define RQ_SIZE 0x18

#endif /* _QUEUE_H */
