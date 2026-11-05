// src/task.h
#ifndef _COTANOS_TASK_H
#define _COTANOS_TASK_H

#include "types.h"

// Forward declarations (żeby kompilator wiedział, że takie struktury istnieją)
struct sched_class;
struct mm_struct;
struct fs_struct;
struct files_struct;
struct signal_struct;
struct thread_struct;
struct cpu_context;
struct sched_entity;
struct sched_rt_entity;
struct sched_dl_entity;

struct sched_statistics {
    u64 wait_start;
    u64 wait_max;
    u64 wait_count;
    u64 wait_sum;
    u64 iowait_count;
    u64 iowait_sum;
    u64 sleep_start;
    u64 sleep_max;
    s64 sum_sleep_runtime;
    u64 block_start;
    u64 block_max;
    u64 exec_max;
    u64 slice_max;
    u64 nr_migrations_cold;
    u64 nr_failed_migrations_affine;
    u64 nr_failed_migrations_running;
    u64 nr_failed_migrations_hot;
    u64 nr_forced_migrations;
    u64 nr_wakeups;
    u64 nr_wakeups_sync;
    u64 nr_wakeups_migrate;
    u64 nr_wakeups_local;
    u64 nr_wakeups_remote;
    u64 nr_wakeups_affine;
    u64 nr_wakeups_affine_attempts;
    u64 nr_wakeups_passive;
    u64 nr_wakeups_idle;
};

struct sched_avg {
    u64 last_update_time;
    u64 load_sum;
    u64 runnable_sum;
    u32 util_sum;
    u32 period_contrib;
    unsigned long load_avg;
    unsigned long runnable_avg;
    unsigned long util_avg;
    unsigned long util_est;
};

struct task_struct {
    /* --- rdzeń schedulera --- */
    pid_t pid;
    pid_t tgid;
    char comm[16];

    const struct sched_class *sched_class;
    int prio;
    int static_prio;
    int normal_prio;
    unsigned int rt_priority;
    unsigned int policy;
    
    struct sched_entity se;
    struct sched_rt_entity rt;
    struct sched_dl_entity dl;

    struct list_head run_list;
    unsigned int on_rq : 1;
    unsigned int on_cpu : 1;
    unsigned int sched_reset_on_fork : 1;
    int cpu;
    int recent_used_cpu;
    unsigned int wake_cpu;

    volatile long state;
    unsigned int preempt_notifiers;
    u64 sched_class_state;

    u64 start_time;
    u64 prev_sum_exec_runtime;
    u64 se_sum_exec_runtime;

    struct cpu_context ctx;
    unsigned long cpus_mask;
    unsigned long *cpus_ptr;
    u64 core_cookie;

    /* --- procesy / wątki --- */
    struct task_struct *parent;
    struct task_struct *real_parent;
    struct task_struct *group_leader;
    struct list_head children;
    struct list_head sibling;
    struct list_head thread_group;
    int exit_state;
    int exit_code;
    unsigned int flags; // PF_*

    /* --- pamięć --- */
    struct mm_struct *mm;
    struct mm_struct *active_mm;
    unsigned long min_flt;
    unsigned long maj_flt;
    unsigned long nswap;
    int pagefault_disabled;
    struct thread_struct thread;

    /* --- system plików --- */
    struct fs_struct *fs;
    struct files_struct *files;

    /* --- sygnały --- */
    struct signal_struct *signal;

    /* --- statystyki i czas --- */
    u64 utime;
    u64 stime;
    u64 gtime;
    u64 nvcsw;
    u64 nivcsw;
    struct sched_statistics stats;
    struct sched_avg avg;

    /* --- tracing / debug --- */
    unsigned long trace_recursion;
    unsigned int lockdep_depth;
};

// To makro w Linuxie zwraca aktualnie wykonujące się zadanie. 
// Na razie zwracamy NULL, zaimplementujesz to później przez rejestr per-CPU.
#define current ((struct task_struct *)0)

#endif