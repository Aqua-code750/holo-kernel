#ifndef VMM_H
#define VMM_H

#include <stdint.h>
#include "pmm.h"

// Page Table/Directory flags
#define I86_PTE_PRESENT  0x01
#define I86_PTE_WRITABLE 0x02
#define I86_PTE_USER     0x04

typedef uint32_t pt_entry;
typedef uint32_t pd_entry;

typedef struct {
    pt_entry m_entries[1024];
} ptable;

typedef struct {
    pd_entry m_entries[1024];
} pdirectory;

void vmm_init(void);

#endif
