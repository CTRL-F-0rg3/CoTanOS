/*
 * ============================================================================
 *  STOR_MM.C - Memory management abstraction implementation
 *  Architecture: RISC-V (RV32 / RV64)
 *
 *  NOTE: This file contains NO actual physical memory allocation. The kernel
 *        is expected to call stor_mm_init() with a vtable that implements
 *        page allocation / MMIO mapping. Until then, every allocating call
 *        returns STOR_ERR_NOT_SUPPORTED.
 *
 *  TODO(kernel-integration): Replace the null-vtable default with a real
 *        PMM-backed implementation once the kernel exposes page frames and
 *        MMIO mapping primitives. The driver already assumes:
 *          - alloc_dma returns physically contiguous, zeroed, DMA-coherent memory
 *          - map_mmio returns an uncached, device-ordered kernel VA
 *          - phys_to_virt / virt_to_phys honour the kernel's direct map
 * ============================================================================
 */

#include "stor_mm.h"

/* ========================================================================== */
/* GLOBAL STATE                                                                */
/* ========================================================================== */

const stor_mm_ops_t *g_stor_mm = NULL;

int stor_mm_init(const stor_mm_ops_t *ops)
{
    g_stor_mm = ops;
    return STOR_OK;
}

bool stor_mm_ready(void)
{
    return g_stor_mm != NULL;
}

/* ========================================================================== */
/* DMA BUFFER ALLOCATION                                                       */
/* ========================================================================== */

static __always_inline size_t stor_dma_normalize_align(size_t align)
{
    if (align == 0) return 4096;
    /* Round up to power of two. */
    if (align & (align - 1)) align = (size_t)stor_roundup_pow2((u32)align);
    if (align < 16) align = 16;       /* sanity floor */
    return align;
}

int stor_dma_alloc(size_t size, size_t align, stor_dma_buf_t *out)
{
    if (!out) return STOR_ERR_INVAL;
    out->virt = NULL;
    out->phys = 0;
    out->size = 0;

    if (!g_stor_mm || !g_stor_mm->alloc_dma) return STOR_ERR_NOT_SUPPORTED;
    if (size == 0) return STOR_ERR_INVAL;

    align = stor_dma_normalize_align(align);

    dma_addr_t phys = 0;
    void *virt = NULL;
    int rc = g_stor_mm->alloc_dma(size, align, &phys, &virt);
    if (rc != STOR_OK) return rc;

    out->virt = virt;
    out->phys = phys;
    out->size = size;
    return STOR_OK;
}

void stor_dma_free(stor_dma_buf_t *buf)
{
    if (!buf) return;
    if (buf->virt && g_stor_mm && g_stor_mm->free_dma)
        g_stor_mm->free_dma(buf->phys, buf->virt, buf->size);
    buf->virt = NULL;
    buf->phys = 0;
    buf->size = 0;
}

/* ========================================================================== */
/* MMIO MAPPING                                                                */
/* ========================================================================== */

int stor_mmio_map(paddr_t phys, size_t size, vaddr_t *out_virt)
{
    if (!out_virt || size == 0) return STOR_ERR_INVAL;
    *out_virt = 0;
    if (!g_stor_mm || !g_stor_mm->map_mmio) return STOR_ERR_NOT_SUPPORTED;
    return g_stor_mm->map_mmio(phys, size, out_virt);
}

void stor_mmio_unmap(vaddr_t virt, size_t size)
{
    if (!virt || !g_stor_mm || !g_stor_mm->unmap_mmio) return;
    g_stor_mm->unmap_mmio(virt, size);
}
