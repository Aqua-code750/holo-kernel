#include "pmm.h"

#define PMM_MAX_BLOCKS 1048576
#define PMM_BLOCKS_PER_BYTE 8
#define PMM_BITMAP_SIZE (PMM_MAX_BLOCKS / 32)

static uint32_t pmm_bitmap[PMM_BITMAP_SIZE];
static uint32_t memory_size = 0;
static uint32_t max_blocks = 0;
static uint32_t used_blocks = 0;

static inline void bitmap_set(int bit) {
    pmm_bitmap[bit / 32] |= (1 << (bit % 32));
}

static inline void bitmap_unset(int bit) {
    pmm_bitmap[bit / 32] &= ~(1 << (bit % 32));
}

static inline int bitmap_test(int bit) {
    return pmm_bitmap[bit / 32] & (1 << (bit % 32));
}

static int pmm_find_first_free(void) {
    for (uint32_t i = 0; i < PMM_BITMAP_SIZE; i++) {
        if (pmm_bitmap[i] != 0xFFFFFFFF) {
            for (int j = 0; j < 32; j++) {
                int bit = 1 << j;
                if (!(pmm_bitmap[i] & bit)) {
                    return i * 32 + j;
                }
            }
        }
    }
    return -1;
}

void pmm_init(multiboot_info_t* mbd) {
    for (uint32_t i = 0; i < PMM_BITMAP_SIZE; i++) {
        pmm_bitmap[i] = 0xFFFFFFFF; // Initially mark everything as used
    }

    if (!(mbd->flags & (1 << 6))) {
        // Memory map not provided
        return;
    }

    multiboot_mmap_entry_t* mmap = (multiboot_mmap_entry_t*)mbd->mmap_addr;
    while ((uint32_t)mmap < mbd->mmap_addr + mbd->mmap_length) {
        if (mmap->type == MULTIBOOT_MEMORY_AVAILABLE) {
            uint32_t addr = (uint32_t)mmap->addr;
            uint32_t len = (uint32_t)mmap->len;

            for (uint32_t i = 0; i < len; i += PMM_FRAME_SIZE) {
                int frame = (addr + i) / PMM_FRAME_SIZE;
                bitmap_unset(frame);
            }
        }
        mmap = (multiboot_mmap_entry_t*)((uint32_t)mmap + mmap->size + sizeof(mmap->size));
    }

    // Reserve first 16MB for kernel, BIOS, and GRUB modules (like DOOM1.WAD)
    for (uint32_t i = 0; i < 0x1000000; i += PMM_FRAME_SIZE) {
        bitmap_set(i / PMM_FRAME_SIZE);
    }

    // Reserve 64MB (from 32MB to 96MB) for the kernel heap (kmalloc)
    for (uint32_t i = 0x02000000; i < 0x06000000; i += PMM_FRAME_SIZE) {
        bitmap_set(i / PMM_FRAME_SIZE);
    }
}

void* pmm_alloc_frame(void) {
    int frame = pmm_find_first_free();
    if (frame == -1) return 0; // Out of memory

    bitmap_set(frame);
    used_blocks++;
    return (void*)(frame * PMM_FRAME_SIZE);
}

void pmm_free_frame(void* addr) {
    uint32_t frame = (uint32_t)addr / PMM_FRAME_SIZE;
    bitmap_unset(frame);
    used_blocks--;
}
