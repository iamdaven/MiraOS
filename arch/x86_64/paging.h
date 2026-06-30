#pragma once

#include <stdint.h>
#include <stddef.h>

#define PAGE_SIZE 4096
#define PAGE_PRESENT (1ULL << 0)
#define PAGE_WRITE   (1ULL << 1)
#define PAGE_SIZE_FLAG (1ULL << 7)

void paging_init(void);
void *paging_map_page(uint64_t virt, uint64_t phys, uint64_t flags);
void paging_unmap_page(uint64_t virt);
uint64_t paging_virt_to_phys(uint64_t virt);
uint64_t paging_heap_base(void);
