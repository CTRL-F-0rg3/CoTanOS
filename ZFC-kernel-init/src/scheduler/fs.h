#ifndef FS_H
#define FS_H
#define ROOTINO 1
#define BSIZE 1024
#define NDIRECT 12
#define NINDIRECT (BSIZE / sizeof(uint))
#define MAXFILE (NDIRECT + NINDIRECT)
struct dirent { unsigned short inum; char name[14]; };
struct inode { int dummy; };
struct superblock { int dummy; };
#endif
