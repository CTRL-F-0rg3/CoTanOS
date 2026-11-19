#ifndef FILE_H
#define FILE_H
enum { FD_NONE, FD_PIPE, FD_INODE, FD_DEVICE };
struct file { int type; int ref; };
struct devsw { int (*read)(int, unsigned long, int); int (*write)(int, unsigned long, int); };
#define NDEV 10
#endif
