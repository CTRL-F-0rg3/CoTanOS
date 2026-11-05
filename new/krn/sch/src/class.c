// src/class.c
#include "sched.h"

// Tablica klas, indeksowana po randze (priorytecie)
static const struct sched_class *sched_classes[NR_SCHED_RANKS];

void sched_register_class(const struct sched_class *cls) {
  if (cls->rank < NR_SCHED_RANKS) {
    sched_classes[cls->rank] = cls;
  }
}

/*
 * Wybór następnego zadania.
 * Iteruje od najwyższego priorytetu (RANK_STOP) w dół.
 * Pierwsza klasa, która zwróci zadanie, wygrywa.
 */
struct task_struct *pick_next_task(struct rq *rq, struct task_struct *prev) {
  const struct sched_class *cls;

  for (int rank = 0; rank < NR_SCHED_RANKS; rank++) {
    cls = sched_classes[rank];
    if (!cls)
      continue;

    struct task_struct *next = cls->pick_next_task(rq, prev);
    if (next) {
      return next;
    }
  }

  // Fallback: jeśli nic nie ma, zwróć zadanie IDLE
  return rq->idle;
}

void enqueue_task(struct rq *rq, struct task_struct *p, int flags) {
  if (p->on_rq)
    return;
  p->sched_class->enqueue_task(rq, p, flags);
  p->on_rq = 1;
  rq->nr_running++;
}

void dequeue_task(struct rq *rq, struct task_struct *p, int flags) {
  if (!p->on_rq)
    return;
  p->sched_class->dequeue_task(rq, p, flags);
  p->on_rq = 0;
  rq->nr_running--;
}

/*
 * Sprawdza, czy nowe zadanie @p powinno wywłaszczyć obecne @rq->curr.
 * Jeśli tak, ustawia flagę TIF_NEED_RESCHED.
 */
void check_preempt_curr(struct rq *rq, struct task_struct *p, int flags) {
  struct task_struct *curr = rq->curr;

  // Jeśli p jest z innej klasy, decyduje ranking.
  if (p->sched_class->rank < curr->sched_class->rank) {
    // Wywłaszcz! (W CotanoS musisz ustawić odpowiednią flagę w p->flags)
    p->flags |= (1 << 1); // Przykład: bit 1 to TIF_NEED_RESCHED
    return;
  }

  // Jeśli ta sama klasa, oddaj decyzję klasie (np. RT porówna priorytety)
  if (p->sched_class->check_preempt_curr) {
    p->sched_class->check_preempt_curr(rq, p, flags);
  }
}
