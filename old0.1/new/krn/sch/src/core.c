// src/core.c
#include "sched.h"
#include "types.h"

/*
 * Boot arg sched_proxy_exec.
 * W prawdziwym Linuxie używa __setup. W CotanoS musisz to podpiąć
 * do swojego parsera argumentów startowych (cmdline).
 */
static int proxy_exec_state = -1; // -1 = UNSPEC, 0 = OFF, 1 = ON

void setup_proxy_exec(const char *str) {
  if (!str || !*str)
    return;

  if (str[0] == '0' || str[0] == 'n' || str[0] == 'N') {
    proxy_exec_state = 0;
    // pr_info("sched_proxy_exec disabled via boot arg\n");
  } else if (str[0] == '1' || str[0] == 'y' || str[0] == 'Y') {
    proxy_exec_state = 1;
    // pr_info("sched_proxy_exec enabled via boot arg\n");
  }
}

/* --- Pomocnicze funkcje do rankingu --- */
static inline const struct sched_dl_entity *
dl_entity_of(const struct task_struct *p) {
  return &p->dl; // Uproszczenie: brak DL servera na tym etapie
}

static inline int dl_prio_cmp(const struct task_struct *a,
                              const struct task_struct *b) {
  s64 da = a->dl.deadline;
  s64 db = b->dl.deadline;
  if (da < db)
    return -1;
  if (da > db)
    return 1;
  return 0;
}

static inline int rt_prio_cmp(const struct task_struct *a,
                              const struct task_struct *b) {
  if (a->prio < b->prio)
    return -1;
  if (a->prio > b->prio)
    return 1;
  return 0;
}

/* --- Główny komparator --- */
int task_prio_cmp(const struct task_struct *a, const struct task_struct *b,
                  bool in_fi) {
  if (a == b)
    return 0;

  // Na razie zakładamy, że ranking jest ustalany na podstawie wskaźnika klasy.
  // W przyszłości zaimplementujesz rank_of(a) i rank_of(b).

  // DLA TESTU: Załóżmy, że jeśli priorytety są różne, to wygrywa mniejszy.
  // (To jest placeholder, docelowo użyj rank_of)
  int ra = a->sched_class ? (int)a->sched_class->rank : RANK_IDLE;
  int rb = b->sched_class ? (int)b->sched_class->rank : RANK_IDLE;

  if (ra != rb)
    return (ra < rb) ? -1 : 1;

  // Ta sama klasa - porównanie wewnątrzklasowe
  if (ra == RANK_DL)
    return dl_prio_cmp(a, b);
  if (ra == RANK_RT)
    return rt_prio_cmp(a, b);

  // Dla FAIR, IDLE, STOP -> na razie remis
  return 0;
}

/* --- Dyspozytor --- */
static const struct sched_class *sched_classes[NR_SCHED_RANKS];

void sched_register_class(const struct sched_class *cls) {
  if (cls->rank < NR_SCHED_RANKS) {
    sched_classes[cls->rank] = cls;
  }
}

// Puste implementacje enqueue/dequeue na start, żeby linker nie płakał
void enqueue_task(struct rq *rq, struct task_struct *p, int flags) {
  // TODO
}

void dequeue_task(struct rq *rq, struct task_struct *p, int flags) {
  // TODO
}

struct task_struct *pick_next_task(struct rq *rq, struct task_struct *prev) {
  // Iteracja po klasach od najwyższego priorytetu
  for (int rank = 0; rank < NR_SCHED_RANKS; rank++) {
    const struct sched_class *cls = sched_classes[rank];
    if (!cls)
      continue;

    struct task_struct *next = cls->pick_next_task(rq, prev);
    if (next)
      return next;
  }

  // Fallback do idle
  return rq->idle;
}
