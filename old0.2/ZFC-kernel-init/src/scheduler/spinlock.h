#ifndef SPINLOCK_H
#define SPINLOCK_H

#include "types.h"

struct cpu;

// Mutual exclusion lock.
struct spinlock {
  uint locked;       // Is the lock held?

  // For debugging:
  char *name;        // Name of lock.
  struct cpu *cpu;   // The cpu holding the lock.
};

#endif
