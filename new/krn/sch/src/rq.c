// src/rq.c
#include "entity.h"
#include "sched.h"

#define MAX_CPUS 64

// Tablica runqueues dla każdego rdzenia
struct rq runqueues[MAX_CPUS];

// Prymityw locka (prosty spinlock na instrukcjach atomowych)
static inline void spin_lock(unsigned long *lock) {
  while (__sync_lock_test_and_set(lock, 1)) {
    while (*lock) {
      __asm__ __volatile__("pause");
    }
  }
}

static inline void spin_unlock(unsigned long *lock) {
  __sync_lock_release(lock);
}

// Pobranie RQ dla obecnego rdzenia (w CotanoS musisz to zaimplementować)
extern int smp_processor_id(void);
#define this_rq() (&runqueues[smp_processor_id()])
#define cpu_rq(cpu) (&runqueues[cpu])

void rq_lock(struct rq *rq) { spin_lock(&rq->lock); }

void rq_unlock(struct rq *rq) { spin_unlock(&rq->lock); }

/*
 * Lockowanie dwóch RQ w kolejności rosnących CPU.
 * Zapobiega deadlockom (ABBA) przy migracjach i balansowaniu.
 */
void double_rq_lock(struct rq *rq1, struct rq *rq2) {
  if (rq1 == rq2) {
    rq_lock(rq1);
  } else if (rq1->cpu < rq2->cpu) {
    rq_lock(rq1);
    rq_lock(rq2);
  } else {
    rq_lock(rq2);
    rq_lock(rq1);
  }
}

void double_rq_unlock(struct rq *rq1, struct rq *rq2) {
  if (rq1 == rq2) {
    rq_unlock(rq1);
  } else {
    rq_unlock(rq1);
    rq_unlock(rq2);
  }
}

// Inicjalizacja RQ przy starcie jądra
void init_rq(int cpu) {
  struct rq *rq = cpu_rq(cpu);
  rq->lock = 0;
  rq->curr = 0;
  rq->idle = 0;
  rq->nr_running = 0;
  rq->cpu = cpu;
  rq->clock = 0;
  rq->rt_nr_running = 0;
  rq->nr_classes = 0;
  rq->rt_queue.next = &rq->rt_queue;
  rq->rt_queue.prev = &rq->rt_queue;
}
