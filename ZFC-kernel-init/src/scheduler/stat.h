#ifndef STAT_H
#define STAT_H
#define T_DIR 1
#define T_FILE 2
#define T_DEVICE 3
struct stat { int dev; unsigned int ino; short type; short nlink; unsigned long size; };
#endif
