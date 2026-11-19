/*
 * ============================================================================
 *  STOR_MM.H - Memory management abstraction for CoTanOS storage drivers
 *  Architecture: RISC-V (RV32 / RV64)
 *
 *  Purpose: Decouple the driver from the kernel's physical memory manager.
 *           The kernel registers a `stor_mm_ops_t` vtable at boot; the
 *           driver then uses stor_dma_alloc / stor_mmio_map. When the vtable
 *           is absent, all ops return STOR_ERR_NOT_SUPPORTED.
 *
 *  TODO(kernel-integration):
 *    - Physical page allocation is currently delegated entirely to ops->alloc_dma.
 *      Once the kernel exposes page frames, wire stor_mm_init() to a real
 *      implementation. The driver MUST remain agnostic to paging / IOMMU.
 *    - MMIO mapping delegates to ops->map_mmio; if IOMMU is present, kernel
 *      should return a bus-visible IOVA there. The driver does not care.
 * ============================================================================
 */

#ifndef STOR_MM_H
#define STOR_MM_H

#include "stor_mmio.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/* MM OPS VTABLE (registered by kernel)                                        */
/* ========================================================================== */

typedef struct {
    /* Allocate physically contiguous DMA buffer (zeroed). */
    int  (*alloc_dma)(size_t size, size_t align, dma_addr_t *phys, void **virt);
    void (*free_dma)(dma_addr_t phys, void *virt, size_t size);

    /* Map MMIO physical range into kernel virtual address space. */
    int  (*map_mmio)(paddr_t phys, size_t size, vaddr_t *virt);
    void (*unmap_mmio)(vaddr_t virt, size_t size);

    /* Phys <-> virt translation for already-mapped regions. */
    void *(*phys_to_virt)(paddr_t phys);
    paddr_t (*virt_to_phys)(const void *virt);

    /* Aligned kernel allocation (for command buffers, queues, PRP lists). */
    void *(*kmalloc_aligned)(size_t size, size_t align);
    void  (*kfree_aligned)(void *ptr, size_t size);
} stor_mm_ops_t;

/* ========================================================================== */
/* INIT / STATE                                                                */
/* ========================================================================== */

int  stor_mm_init(const stor_mm_ops_t *ops);   /* pass NULL to reset to "unavailable" */
bool stor_mm_ready(void);

/* ========================================================================== */
/* DMA BUFFER HANDLE                                                           */
/* ========================================================================== */

typedef struct {
    void      *virt;   /* kernel virtual address (CPU-accessible) */
    dma_addr_t phys;   /* bus-visible DMA address                */
    size_t     size;
} stor_dma_buf_t;

#define STOR_DMA_BUF_INIT { NULL, 0, 0 }

/* Allocate a physically contiguous, aligned, zeroed DMA buffer.
 * align: 0 or a power of two; 0 => default 4096.
 * On success writes phys, virt, size; returns STOR_OK. */
int  stor_dma_alloc(size_t size, size_t align, stor_dma_buf_t *out);

/* Free a previously allocated buffer. Safe on zeroed handle. */
void stor_dma_free(stor_dma_buf_t *buf);

/* Cached phys/virt accessors (must be called after alloc). */
static __always_inline dma_addr_t stor_dma_phys(const stor_dma_buf_t *b) { return b->phys; }
static __always_inline void      *stor_dma_virt(const stor_dma_buf_t *b) { return b->virt; }

/* ========================================================================== */
/* MMIO MAPPING                                                                */
/* ========================================================================== */

int  stor_mmio_map(paddr_t phys, size_t size, vaddr_t *out_virt);
void stor_mmio_unmap(vaddr_t virt, size_t size);

/* ========================================================================== */
/* PHYS <-> VIRT (only for ranges already mapped by the kernel)                */
/* ========================================================================== */

static __always_inline void *stor_phys_to_virt(paddr_t phys)
{
    extern const stor_mm_ops_t *g_stor_mm;
    if (!g_stor_mm || !g_stor_mm->phys_to_virt) return NULL;
    return g_stor_mm->phys_to_virt(phys);
}

static __always_inline paddr_t stor_virt_to_phys(const void *virt)
{
    extern const stor_mm_ops_t *g_stor_mm;
    if (!g_stor_mm || !g_stor_mm->virt_to_phys) return 0;
    return g_stor_mm->virt_to_phys(virt);
}

#ifdef __cplusplus
}
#endif

#endif /* STOR_MM_H */
