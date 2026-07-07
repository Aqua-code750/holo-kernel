#include "vmm.h"

// Note: In a real kernel, memset is in a string library.
// We'll declare a simple one here.
static void vmm_memset(void *ptr, int value, int size) {
    char *c = (char *)ptr;
    for (int i = 0; i < size; ++i) c[i] = (char)value;
}

static pdirectory* current_pd = 0;

void vmm_init(void) {
    // 1. Allocate page directory
    pdirectory* pd = (pdirectory*)pmm_alloc_frame();
    if (!pd) return; // Out of memory
    vmm_memset(pd, 0, sizeof(pdirectory));

    // 2. Allocate one page table to identity map the first 4MB
    ptable* pt = (ptable*)pmm_alloc_frame();
    if (!pt) return;
    vmm_memset(pt, 0, sizeof(ptable));

    // 3. Map the first 4MB of memory (1024 pages * 4KB = 4MB)
    for (int i = 0; i < 1024; i++) {
        pt_entry page = (i * 4096) | I86_PTE_PRESENT | I86_PTE_WRITABLE;
        pt->m_entries[i] = page;
    }

    // 4. Add the page table to the page directory
    pd_entry table = (uint32_t)pt | I86_PTE_PRESENT | I86_PTE_WRITABLE;
    pd->m_entries[0] = table;

    current_pd = pd;

    // 5. Load the page directory into CR3
    __asm__ volatile("mov %0, %%cr3" : : "r"(current_pd));

    // 6. Enable paging in CR0
    uint32_t cr0;
    __asm__ volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000; // Set PG bit
    __asm__ volatile("mov %0, %%cr0" : : "r"(cr0));
}
