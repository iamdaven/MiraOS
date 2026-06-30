#include "heap.h"
#include "arch/x86_64/paging.h"
#include "kernel/mem.h"
#include "kernel/panic.h"

#define HEAP_SIZE (16 * 1024 * 1024)
#define MIN_BLOCK 32
#define HEADER_SIZE sizeof(struct block_header)

struct block_header {
    size_t size;
    int free;
    struct block_header *next;
    struct block_header *prev;
};

static struct block_header *heap_head;
static uint64_t heap_mapped;
static uint64_t heap_virt_base;

static void heap_map_more(uint64_t needed) {
    while (heap_mapped < needed) {
        void *phys = pmm_alloc_page();
        if (!phys)
            panic("heap oom");
        paging_map_page(heap_virt_base + heap_mapped, (uint64_t)phys, 0);
        heap_mapped += 4096;
    }
}

void heap_init(void) {
    heap_virt_base = paging_heap_base();
    heap_mapped = 4096;
    heap_map_more(4096);

    heap_head = (struct block_header *)heap_virt_base;
    heap_head->size = 4096 - HEADER_SIZE;
    heap_head->free = 1;
    heap_head->next = 0;
    heap_head->prev = 0;
}

static struct block_header *split_block(struct block_header *block, size_t size) {
    if (block->size < size + HEADER_SIZE + MIN_BLOCK)
        return block;

    struct block_header *new_block = (struct block_header *)((uint8_t *)block + HEADER_SIZE + size);
    new_block->size = block->size - size - HEADER_SIZE;
    new_block->free = 1;
    new_block->next = block->next;
    new_block->prev = block;
    if (block->next)
        block->next->prev = new_block;
    block->next = new_block;
    block->size = size;
    return block;
}

static void merge_adjacent(struct block_header *block) {
    if (block->next && block->next->free) {
        block->size += HEADER_SIZE + block->next->size;
        block->next = block->next->next;
        if (block->next)
            block->next->prev = block;
    }
    if (block->prev && block->prev->free) {
        block->prev->size += HEADER_SIZE + block->size;
        block->prev->next = block->next;
        if (block->next)
            block->next->prev = block->prev;
    }
}

void *kmalloc(size_t size) {
    if (size == 0)
        return 0;

    size = (size + 15) & ~15;
    struct block_header *current = heap_head;

    while (current) {
        if (current->free && current->size >= size) {
            current = split_block(current, size);
            current->free = 0;
            return (uint8_t *)current + HEADER_SIZE;
        }
        current = current->next;
    }

    uint64_t needed = heap_mapped + 4096;
    heap_map_more(needed);

    struct block_header *new_area = (struct block_header *)(heap_virt_base + heap_mapped - 4096);
    if (heap_head) {
        struct block_header *tail = heap_head;
        while (tail->next)
            tail = tail->next;
        tail->next = new_area;
        new_area->prev = tail;
    } else {
        heap_head = new_area;
    }
    new_area->size = 4096 - HEADER_SIZE;
    new_area->free = 1;
    new_area->next = 0;

    return kmalloc(size);
}

void kfree(void *ptr) {
    if (!ptr)
        return;
    struct block_header *block = (struct block_header *)((uint8_t *)ptr - HEADER_SIZE);
    block->free = 1;
    merge_adjacent(block);
}

void *krealloc(void *ptr, size_t size) {
    if (!ptr)
        return kmalloc(size);
    if (size == 0) {
        kfree(ptr);
        return 0;
    }
    struct block_header *block = (struct block_header *)((uint8_t *)ptr - HEADER_SIZE);
    if (block->size >= size)
        return ptr;
    void *new_ptr = kmalloc(size);
    if (!new_ptr)
        return 0;
    for (size_t i = 0; i < block->size; i++)
        ((uint8_t *)new_ptr)[i] = ((uint8_t *)ptr)[i];
    kfree(ptr);
    return new_ptr;
}
