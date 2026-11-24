/*
 * CoTanOS shim layer for Linux DRM code
 * Provides compatibility definitions to adapt Linux DRM code to CoTanOS
 */

#ifndef _COTAN_SHIM_H
#define _COTAN_SHIM_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* Basic types mapping */
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

/* Boolean */
#ifndef bool
typedef _Bool bool;
#define true 1
#define false 0
#endif

/* Error codes */
#define EINVAL 22
#define ENOMEM 12
#define ENODEV 19
#define EIO 5

/* Simple logging macros */
#define pr_info(fmt, ...) printf("[drm] " fmt, ##__VA_ARGS__)
#define pr_err(fmt, ...) printf("[drm] ERROR: " fmt, ##__VA_ARGS__)
#define pr_warn(fmt, ...) printf("[drm] WARN: " fmt, ##__VA_ARGS__)
#define pr_debug(fmt, ...) printf("[drm] DEBUG: " fmt, ##__VA_ARGS__)

#define DRM_INFO(fmt, ...) pr_info(fmt, ##__VA_ARGS__)
#define DRM_ERROR(fmt, ...) pr_err(fmt, ##__VA_ARGS__)
#define DRM_DEBUG(fmt, ...) pr_debug(fmt, ##__VA_ARGS__)

/* Memory allocation stubs - will be implemented later */
#define kmalloc(size, flags) malloc(size)
#define kzalloc(size, flags) calloc(1, size)
#define kfree(ptr) free(ptr)
#define vmalloc(size) malloc(size)
#define vfree(ptr) free(ptr)

#define GFP_KERNEL 0
#define GFP_ATOMIC 0

/* String functions */
#define kstrdup(str, flags) strdup(str)

/* Math helpers */
#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))
#define clamp(val, lo, hi) min(max(val, lo), hi)

#define DIV_ROUND_UP(n, d) (((n) + (d) - 1) / (d))
#define ALIGN(x, a) (((x) + (a) - 1) & ~((a) - 1))

/* Bit operations */
#define BIT(nr) (1UL << (nr))
#define BITS_PER_LONG 64

/* List stubs - simplified for now */
struct list_head {
    struct list_head *next, *prev;
};

#define LIST_HEAD_INIT(name) { &(name), &(name) }
#define LIST_HEAD(name) struct list_head name = LIST_HEAD_INIT(name)

static inline void INIT_LIST_HEAD(struct list_head *list) {
    list->next = list;
    list->prev = list;
}

/* Mutex stubs */
struct mutex {
    int locked;
};

#define mutex_init(m) do { (m)->locked = 0; } while(0)
#define mutex_lock(m) do { (m)->locked = 1; } while(0)
#define mutex_unlock(m) do { (m)->locked = 0; } while(0)

/* Spinlock stubs */
struct spinlock {
    int locked;
};

#define spin_lock_init(l) do { (l)->locked = 0; } while(0)
#define spin_lock(l) do { (l)->locked = 1; } while(0)
#define spin_unlock(l) do { (l)->locked = 0; } while(0)

/* Device structure stub */
struct device {
    const char *name;
};

/* Module macros - no-op for us */
#define MODULE_LICENSE(x)
#define MODULE_AUTHOR(x)
#define MODULE_DESCRIPTION(x)
#define EXPORT_SYMBOL(x)
#define EXPORT_SYMBOL_GPL(x)

#endif /* _COTAN_SHIM_H */
