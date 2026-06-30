#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define MULTIBOOT2_MAGIC 0x36d76289

struct multiboot_tag {
    uint32_t type;
    uint32_t size;
};

struct multiboot_mmap_entry {
    uint64_t base;
    uint64_t length;
    uint32_t type;
    uint32_t zero;
};

struct multiboot_tag_framebuffer {
    uint32_t type;
    uint32_t size;
    uint64_t framebuffer_addr;
    uint32_t framebuffer_pitch;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint8_t framebuffer_bpp;
    uint8_t framebuffer_type;
    uint8_t reserved;
};

void kernel_main(uint64_t magic, uint64_t mbi);
void kernel_idle(void);
