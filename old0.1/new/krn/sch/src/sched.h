// src/sched.h
#ifndef _COTANOS_SCHED_H
#define _COTANOS_SCHED_H

#include "types.h"
#include "task.h"

/* --- abstrakcja pojedynczego rdzenia --- */
struct rq {
    unsigned long lock;
    struct task_struct *curr;
    struct task_struct *idle;
    unsigned int nr_running;
    int cpu;
    u64 clock;
    struct list_head rt_queue;
    unsigned int rt_nr_running;
    const struct sched_class *classes[6]; // Zależnie od NR_SCHED_RANKS
    unsigned int nr_classes;
};

/* --- enum sched_rank --- */
enum sched_rank {
    RANK_STOP = 0,
    RANK_DL,
    RANK_RT,
    RANK_FAIR,
    RANK_EXT,
    RANK_IDLE,
    NR_SCHED_RANKS,
};

/* --- VTable dla klas --- */
struct sched_class {
    const char *name;
    enum sched_rank rank;
    void (*enqueue_task)(struct rq *rq, struct task_struct *p, int flags);
    void (*dequeue_task)(struct rq *rq, struct task_struct *p, int flags);
    struct task_struct *(*pick_next_task)(struct rq *rq, struct task_struct *prev);
    void (*put_prev_task)(struct rq *rq, struct task_struct *prev);
    void (*set_next_task)(struct rq *rq, struct task_struct *next);
    void (*check_preempt_curr)(struct rq *rq, struct task_struct *p, int flags);
};

/* --- Porównywanie priorytetów --- */
int task_prio_cmp(const struct task_struct *a, const struct task_struct *b, bool in_fi);

static inline bool prio_less(const struct task_struct *a, const struct task_struct *b, bool in_fi) {
    return task_prio_cmp(a, b, in_fi) > 0;
}

/* --- Dyspozytor --- */
void sched_register_class(const struct sched_class *cls);
struct task_struct *pick_next_task(struct rq *rq, struct task_struct *prev);

#endif