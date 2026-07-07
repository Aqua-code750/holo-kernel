#ifndef PMM_H
#define PMM_H

#include <stdint.h>
#include "multiboot.h"

#define PMM_FRAME_SIZE 4096

void pmm_init(multiboot_info_t* mbd);
void* pmm_alloc_frame(void);
void pmm_free_frame(void* addr);

#endif
