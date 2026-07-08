#include "vmm.h"
#include "pmm.h"

// Note: In a real kernel, memset is in a string library.
// We'll declare a simple one here.
static void vmm_memset(void *ptr, int value, int size) {
    char *c = (char *)ptr;
    for (int i = 0; i < size; ++i) c[i] = (char)value;
}

static pdirectory* current_pd = 0;
extern uint32_t* fb;

void vmm_init(void) {
    // 1. Allocate page directory
    pdirectory* pd = (pdirectory*)pmm_alloc_frame();
    if (!pd) return; // Out of memory
    vmm_memset(pd, 0, sizeof(pdirectory));

    // 2. Allocate and map the first 32MB of memory (8 page tables * 4MB each)
    for (int t = 0; t < 8; t++) {
        ptable* pt = (ptable*)pmm_alloc_frame();
        if (!pt) return;
        vmm_memset(pt, 0, sizeof(ptable));

        for (int i = 0; i < 1024; i++) {
            pt_entry page = ((t * 1024 + i) * 4096) | I86_PTE_PRESENT | I86_PTE_WRITABLE;
            pt->m_entries[i] = page;
        }

        pd_entry table = (uint32_t)pt | I86_PTE_PRESENT | I86_PTE_WRITABLE;
        pd->m_entries[t] = table;
    }
    
    // Map the Framebuffer if it exists (map 16MB)
    if (fb) {
        uint32_t fb_phys = (uint32_t)fb;
        uint32_t fb_pd_idx = fb_phys / 0x400000;
        
        for (int t = 0; t < 4; t++) {
            ptable* pt = (ptable*)pmm_alloc_frame();
            if (!pt) return;
            vmm_memset(pt, 0, sizeof(ptable));
            
            for (int i = 0; i < 1024; i++) {
                pt_entry page = (fb_phys + t * 0x400000 + i * 4096) | I86_PTE_PRESENT | I86_PTE_WRITABLE;
                pt->m_entries[i] = page;
            }
            pd_entry table = (uint32_t)pt | I86_PTE_PRESENT | I86_PTE_WRITABLE;
            pd->m_entries[fb_pd_idx + t] = table;
        }
    }

    current_pd = pd;

    // 5. Load the page directory into CR3
    __asm__ volatile("mov %0, %%cr3" : : "r"(current_pd));

    // 6. Enable paging in CR0
    uint32_t cr0;
    __asm__ volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000; // Set PG bit
    __asm__ volatile("mov %0, %%cr0" : : "r"(cr0));
}
