#ifndef SLEEPLOCK_H
#define SLEEPLOCK_H
struct sleeplock { int dummy; };
void acquiresleep(struct sleeplock*);
void releasesleep(struct sleeplock*);
int holdingsleep(struct sleeplock*);
void initsleeplock(struct sleeplock*, char*);
#endif
