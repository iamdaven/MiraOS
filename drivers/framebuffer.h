#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint64_t addr;
    uint32_t pitch;
    uint32_t width;
    uint32_t height;
    uint8_t bpp;
} framebuffer_t;

bool fb_init(uint64_t mbi);
bool fb_ready(void);
framebuffer_t *fb_info(void);
void fb_put_pixel(uint32_t x, uint32_t y, uint32_t color);
void fb_fill_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color);
