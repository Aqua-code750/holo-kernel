#include "kheap.h"

// We reserved 64MB in pmm.c for the heap
#define HEAP_START 0x02000000 // 32MB
#define HEAP_SIZE  0x04000000 // 64MB

typedef struct heap_block {
    size_t size;
    uint8_t is_free;
    struct heap_block* next;
} heap_block_t;

static heap_block_t* head = (heap_block_t*)HEAP_START;

void kheap_init(void) {
    head->size = HEAP_SIZE - sizeof(heap_block_t);
    head->is_free = 1;
    head->next = 0;
}

static void merge_free_blocks(void) {
    heap_block_t* current = head;
    while (current != 0 && current->next != 0) {
        if (current->is_free && current->next->is_free) {
            current->size += current->next->size + sizeof(heap_block_t);
            current->next = current->next->next;
        } else {
            current = current->next;
        }
    }
}

void* kmalloc(size_t size) {
    if (size == 0) return 0;
    
    // Align allocation size to 4 bytes
    if (size % 4 != 0) {
        size += 4 - (size % 4);
    }

    heap_block_t* current = head;
    while (current != 0) {
        if (current->is_free && current->size >= size) {
            // Split block if there's enough room for another block header and some data
            if (current->size > size + sizeof(heap_block_t) + 4) {
                heap_block_t* new_block = (heap_block_t*)((uint8_t*)current + sizeof(heap_block_t) + size);
                new_block->size = current->size - size - sizeof(heap_block_t);
                new_block->is_free = 1;
                new_block->next = current->next;
                
                current->size = size;
                current->next = new_block;
            }
            current->is_free = 0;
            return (void*)((uint8_t*)current + sizeof(heap_block_t));
        }
        current = current->next;
    }
    return 0; // Out of memory
}

void kfree(void* ptr) {
    if (!ptr) return;
    heap_block_t* block = (heap_block_t*)((uint8_t*)ptr - sizeof(heap_block_t));
    block->is_free = 1;
    merge_free_blocks();
}

void* krealloc(void* ptr, size_t size) {
    if (!ptr) return kmalloc(size);
    if (size == 0) {
        kfree(ptr);
        return 0;
    }
    
    heap_block_t* block = (heap_block_t*)((uint8_t*)ptr - sizeof(heap_block_t));
    if (block->size >= size) {
        return ptr; // Already large enough
    }
    
    void* new_ptr = kmalloc(size);
    if (!new_ptr) return 0;
    
    // Copy old data
    char* src = (char*)ptr;
    char* dst = (char*)new_ptr;
    for (size_t i = 0; i < block->size; i++) {
        dst[i] = src[i];
    }
    
    kfree(ptr);
    return new_ptr;
}
