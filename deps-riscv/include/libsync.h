#ifndef LIBSYNC_H
#define LIBSYNC_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Stub implementations for sync_file framework */
static inline int sync_file_create(int fd) { return fd; }
static inline int sync_wait(int fd, int timeout) { return 0; }
static inline int sync_merge(const char *name, int fd1, int fd2) { return fd1; }

static inline void sync_accumulate(const char *name, int *out_fd, int in_fd) {
    /* Stub: In real implementation this merges sync fences. 
       Here we just keep the first valid fd. */
    if (*out_fd == -1) {
        *out_fd = in_fd;
    }
}

#ifdef __cplusplus
}
#endif

#endif /* LIBSYNC_H */
