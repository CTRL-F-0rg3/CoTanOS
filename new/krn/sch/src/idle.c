// src/idle.c
#include "sched.h"

static void idle_enqueue(struct rq *rq, struct task_struct *p, int flags) {
  // IDLE zawsze jest na RQ, nie robimy nic
}

static void idle_dequeue(struct rq *rq, struct task_struct *p, int flags) {
  // IDLE nigdy nie schodzi z RQ
}

static struct task_struct *idle_pick_next(struct rq *rq,
                                          struct task_struct *prev) {
  return rq->idle; // Zawsze gotowy do powrotu
}

static void idle_put_prev(struct rq *rq, struct task_struct *prev) {}
static void idle_set_next(struct rq *rq, struct task_struct *next) {}

const struct sched_class idle_sched_class = {
    .name = "idle",
    .rank = RANK_IDLE,
    .enqueue_task = idle_enqueue,
    .dequeue_task = idle_dequeue,
    .pick_next_task = idle_pick_next,
    .put_prev_task = idle_put_prev,
    .set_next_task = idle_set_next,
};
