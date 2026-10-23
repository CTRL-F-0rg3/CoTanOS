#include "paging.h"
#include "pml.h"
#include "tlb.h"
#include "../arch/x86_64/paging.h"
#include "../arch/x86_64/memory.h"
#include "../alloc/physical/pmm.h"
#include "../alloc/heap/heap.h"
#include "../arch/x86_64/tlb.h"
#include "../core/range.h"

void kprintf(const char *fmt, ...);

static uint64_t paging_kernel_pml4 = 0;
static bool paging_subsystem_ready = false;

static uint64_t prot_to_pte(uint32_t prot)
{
	uint64_t pte = PTE_PRESENT;

	if (prot & PROT_WRITE) {
	    pte |= PTE_WRITABLE;
	}

	if (!(prot & PROT_EXEC)) {
	    pte |= PTE_NX;
	}

	if (prot & PROT_USER) {
	    pte |= PTE_USER;
	}

	if (prot & PROT_DEVICE) {
	    pte |= PTE_CACHE_DISABLE | PTE_WRITE_THROUGH | PTE_NX;
	}

	return pte;
}

static bool page_range_ok(uint64_t addr, size_t len)
{
	uint64_t start, end;

	if (!range_from_addr_len(addr, (uint64_t)len, ARCH_PAGE_SIZE,
	                         0, UINT64_MAX, false, &start, &end)) {
	    return false;
	}

	return start == addr && end == addr + (uint64_t)len;
}

bool paging_subsystem_init(void)
{
	if (paging_subsystem_ready) {
	    return true;
	}

	paging_kernel_pml4 = paging_read_cr3();
	paging_subsystem_ready = true;

	return true;
}

static void free_tables(uint64_t table_phys, int level)
{
	uint64_t *table = pml_table_ptr(table_phys);

	if (level > PML_LEVEL_PT) {
	    for (size_t i = 0; i < PML_ENTRIES; i++) {
	        uint64_t e = table[i];

	        if (pml_entry_present(e) && !pml_entry_large(e)) {
	            free_tables(pml_entry_addr(e), level - 1);
	        }
	    }
	}

	pmm_free_frame(table_phys);
}

/*
 * Mark every intermediate (non-leaf) page-table entry in the shared low half
 * as user-accessible. The kernel's identity map was built by the bootloader
 * with supervisor-only intermediate tables, so a ring-3 page walk would fault
 * on them before ever reaching the leaf PTEs the user mappings created (which
 * do carry PTE_USER). Leaf entries — the large pages and the final PTEs — are
 * left exactly as they are: their own USER bit is what gates the final access,
 * so the kernel's identity-mapped pages stay supervisor-only.
 */
static void mark_intermediate_user(uint64_t pml4_phys)
{
	uint64_t *pml4 = pml_table_ptr(pml4_phys);

	for (size_t i = 0; i < 256; i++) {
	    uint64_t e = pml4[i];

	    if (!pml_entry_present(e) || pml_entry_large(e)) {
	        continue;
	    }

	    pml4[i] |= PTE_USER;

	    uint64_t *pdpt = pml_table_ptr(pml_entry_addr(e));

	    for (size_t j = 0; j < PML_ENTRIES; j++) {
	        uint64_t pe = pdpt[j];

	        if (!pml_entry_present(pe) || pml_entry_large(pe)) {
	            continue;
	        }

	        pdpt[j] |= PTE_USER;

	        uint64_t *pd = pml_table_ptr(pml_entry_addr(pe));

	        for (size_t k = 0; k < PML_ENTRIES; k++) {
	            uint64_t pde = pd[k];

	            if (!pml_entry_present(pde) || pml_entry_large(pde)) {
	                continue;
	            }

	            pd[k] |= PTE_USER;
	        }
	    }
	}
}


address_space_t *paging_aspace_create(void)
{
	if (!paging_subsystem_ready) {
	    return NULL;
	}

	uint64_t pml4_phys = 0;

	if (!pmm_alloc_zero_frame(&pml4_phys)) {
	    return NULL;
	}

	uint64_t *new_pml4 = pml_table_ptr(pml4_phys);
	uint64_t *kernel_pml4 = pml_table_ptr(paging_kernel_pml4);

	/*
	 * Share the kernel's *whole* page table, not just the higher half.
	 * The kernel is identity-mapped in the low half (the bootloader loads it
	 * at physical 0x400000 with `IdentityMappedAddr`), so the ring-switch
	 * trampoline (tr_restore_ctx, in the kernel's .text) is only reachable
	 * after `write_cr3(user_pml4)` if the low half is mapped too. Copying only
	 * entries 256..511 left the low half empty and the first switch to a user
	 * address space unmapped the running kernel code.
	 *
	 * The ring-3 ELF is linked above the kernel image (see ustd/linker.ld) so
	 * the user mappings never collide with these shared low-half entries.
	 */
	for (size_t i = 0; i < 512; i++) {
	    new_pml4[i] = kernel_pml4[i];
	}

	mark_intermediate_user(pml4_phys);

	address_space_t *as = (address_space_t *)heap_alloc(sizeof(address_space_t));

	if (as == NULL) {
	    pmm_free_frame(pml4_phys);
	    return NULL;
	}

	as->pml4_phys = pml4_phys;
	as->kernel = false;

	return as;
}

void paging_aspace_destroy(address_space_t *as)
{
	if (as == NULL || as->kernel) {
	    return;
	}

	/*
	 * The low half of the PML4 is shared with the kernel (see
	 * paging_aspace_create), so it must not be freed here: freeing the shared
	 * PDPT/PD/PT would tear down the kernel's own identity map. The user's own
	 * tables (created underneath the shared PDPT/PD for the ELF image and the
	 * mmap regions) are not tracked separately and simply leak on destroy.
	 */
	pmm_free_frame(as->pml4_phys);

	heap_free(as);
}

void paging_aspace_switch(address_space_t *as)
{
	if (as == NULL) {
	    return;
	}

	paging_write_cr3(as->pml4_phys);
	tlb_flush_all();
}

uint64_t paging_aspace_cr3(const address_space_t *as)
{
	if (as == NULL) {
	    return paging_kernel_pml4;
	}

	return as->pml4_phys;
}

bool paging_aspace_map(address_space_t *as,
	                   uint64_t virt,
	                   uint64_t phys,
	                   size_t len,
	                   uint32_t prot)
{
	if (as == NULL || len == 0) {
	    return false;
	}

	kprintf("[asmap] virt=0x%llx phys=0x%llx len=0x%zx prot=0x%x\n",
	        (unsigned long long)virt, (unsigned long long)phys, len, prot);

	if (!page_range_ok(virt, len) || !page_range_ok(phys, len)) {
	    return false;
	}

	uint64_t pml4 = as->pml4_phys;
	uint64_t pte = prot_to_pte(prot);

	size_t pages = (size_t)(len / ARCH_PAGE_SIZE);
	size_t mapped = 0;

	for (size_t i = 0; i < pages; i++) {
	    uint64_t v = virt + (uint64_t)i * ARCH_PAGE_SIZE;
	    uint64_t p = phys + (uint64_t)i * ARCH_PAGE_SIZE;

	    if (!paging_map_page_in(pml4, v, p, pte)) {
	        for (size_t j = 0; j < mapped; j++) {
	            paging_unmap_page_in(pml4, virt + (uint64_t)j * ARCH_PAGE_SIZE);
	        }

	        return false;
	    }

	    mapped++;
	}

	return true;
}

bool paging_aspace_unmap(address_space_t *as, uint64_t virt, size_t len)
{
	if (as == NULL || len == 0) {
	    return false;
	}

	if (!page_range_ok(virt, len)) {
	    return false;
	}

	uint64_t pml4 = as->pml4_phys;

	tlb_batch_t batch;
	tlb_batch_begin(&batch);

	size_t pages = (size_t)(len / ARCH_PAGE_SIZE);

	for (size_t i = 0; i < pages; i++) {
	    uint64_t v = virt + (uint64_t)i * ARCH_PAGE_SIZE;

	    if (paging_unmap_page_in(pml4, v)) {
	        tlb_batch_add(&batch, v);
	    }
	}

	tlb_batch_commit(&batch);

	return true;
}

bool paging_aspace_protect(address_space_t *as,
	                       uint64_t virt,
	                       size_t len,
	                       uint32_t prot)
{
	if (as == NULL || len == 0) {
	    return false;
	}

	if (!page_range_ok(virt, len)) {
	    return false;
	}

	uint64_t pml4 = as->pml4_phys;
	uint64_t pte = prot_to_pte(prot);

	size_t pages = (size_t)(len / ARCH_PAGE_SIZE);

	for (size_t i = 0; i < pages; i++) {
	    uint64_t v = virt + (uint64_t)i * ARCH_PAGE_SIZE;

	    if (!paging_is_mapped_in(pml4, v)) {
	        return false;
	    }
	}

	tlb_batch_t batch;
	tlb_batch_begin(&batch);

	for (size_t i = 0; i < pages; i++) {
	    uint64_t v = virt + (uint64_t)i * ARCH_PAGE_SIZE;

	    if (!paging_set_flags_in(pml4, v, pte)) {
	        tlb_batch_commit(&batch);
	        return false;
	    }

	    tlb_batch_add(&batch, v);
	}

	tlb_batch_commit(&batch);

	return true;
}

uint64_t paging_aspace_translate(address_space_t *as, uint64_t virt)
{
	if (as == NULL) {
	    return paging_translate_in(paging_kernel_pml4, virt);
	}

	return paging_translate_in(as->pml4_phys, virt);
}

bool paging_kernel_map(uint64_t virt, uint64_t phys, size_t len, uint32_t prot)
{
	if (!paging_subsystem_ready) {
	    return false;
	}

	return paging_aspace_map((address_space_t *)&(address_space_t){
	                             .pml4_phys = paging_kernel_pml4,
	                             .kernel = true,
	                         },
	                         virt, phys, len, prot);
}

bool paging_kernel_unmap(uint64_t virt, size_t len)
{
	if (!paging_subsystem_ready) {
	    return false;
	}

	return paging_aspace_unmap((address_space_t *)&(address_space_t){
	                               .pml4_phys = paging_kernel_pml4,
	                               .kernel = true,
	                           },
	                           virt, len);
}