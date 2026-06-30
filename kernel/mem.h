#pragma once

#include <stddef.h>
#include <stdint.h>

void pmm_init(uint64_t mbi);
void *pmm_alloc_page(void);
void pmm_free_page(void *page);
uint64_t pmm_total_pages(void);
uint64_t pmm_free_count(void);
