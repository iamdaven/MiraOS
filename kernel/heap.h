#pragma once

#include <stddef.h>
#include <stdint.h>

void heap_init(void);
void *kmalloc(size_t size);
void kfree(void *ptr);
void *krealloc(void *ptr, size_t size);
void *kcalloc(size_t nmemb, size_t size);
size_t heap_used(void);
size_t heap_total(void);
