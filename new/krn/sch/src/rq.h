// src/rq.h - runqueue w stylu Linuksa dla RISC-V (RV64, RVWMO),
// skalowalna 1..128 CPU
#pragma once
#include <stdint.h>

#ifndef NR_CPUS
#define NR_CPUS 128 /* limit kompilacji; 90 rdzeni mieści się z zapasem */
#endif

#define CACHELINE 64
#define ____cacheline_aligned __attribute__((aligned(CACHELINE)))

#define MAX_RT_PRIO 100
#define RT_BITMAP_WORDS ((MAX_RT_PRIO + 63) / 64)
#define MAX_SCHED_CLASSES 4

struct list_head {
  struct list_head *next, *prev;
};

/* ---- Ticket spinlock: FIFO-fair, bez głodzenia przy 90 rdzeniach ---- */
typedef union {
  uint32_t whole;
  struct {
    volatile uint16_t owner; /* little-endian RISC-V: dolne 16 bitów */
    volatile uint16_t next;
  };
} ticket_lock_t;

/* ---- Domeny schedulingu (topologia): SMT -> klaster/LLC -> NUMA ---- */
#define SD_LEVELS 3
struct sched_domain {
  uint64_t span[(NR_CPUS + 63) / 64]; /* maska CPU w domenie */
  uint32_t balance_interval_us;       /* rzadziej na wyższych poziomach */
  uint64_t next_balance;
};

struct rq;
struct task; /* z entity.h */

struct sched_class {
  void (*enqueue)(struct rq *, struct task *);
  void (*dequeue)(struct rq *, struct task *);
  struct task *(*pick_next)(struct rq *);
  void (*tick)(struct rq *, struct task *);
};

/* Priorytet RT: bitmapa + tablica list => pick_next w O(1) przez ctz */
struct rt_prio_array {
  uint64_t bitmap[RT_BITMAP_WORDS];
  struct list_head queue[MAX_RT_PRIO];
};

/*
 * Każdy rq zajmuje własne linie cache. Pola gorące (lock, nr_running, curr)
 * trzymamy razem, a dane czytane przez INNE rdzenie (load balancer)
 * osobno, żeby nie powodować false sharingu.
 */
struct rq {
  /* --- gorące, pisane przez właściciela --- */
  ticket_lock_t lock;
  uint32_t cpu;
  uint32_t nr_running;
  struct task *curr;
  struct task *idle;
  uint64_t clock;
  uint32_t need_resched;
  uint32_t rt_nr_running;
  struct rt_prio_array rt;
  const struct sched_class *classes[MAX_SCHED_CLASSES];
  uint32_t nr_classes;

  /* --- czytane lock-free przez innych (osobna linia cache) --- */
  ____cacheline_aligned uint32_t load_avg; /* READ_ONCE przy balansowaniu */
  uint32_t nr_running_lockless;            /* kopia do szybkiego skanu */
  struct sched_domain *sd[SD_LEVELS];
  uint64_t last_balance;
} ____cacheline_aligned;

extern uint32_t nr_cpu_ids; /* faktyczna liczba CPU wykryta przy starcie */
extern struct rq runqueues[NR_CPUS];

#define cpu_rq(cpu) (&runqueues[(cpu)])
extern int smp_processor_id(void);
#define this_rq() cpu_rq(smp_processor_id())

void init_rq(int cpu);
void rq_lock(struct rq *rq);
void rq_unlock(struct rq *rq);
unsigned long rq_lock_irqsave(struct rq *rq);
void rq_unlock_irqrestore(struct rq *rq, unsigned long flags);
void double_rq_lock(struct rq *a, struct rq *b);
void double_rq_unlock(struct rq *a, struct rq *b);
void update_rq_clock(struct rq *rq);
struct rq *find_busiest_rq(struct rq *this_rq, int level);
void rq_enqueue_rt(struct rq *rq, struct list_head *node, unsigned prio);
struct list_head *rq_pick_rt(struct rq *rq);
