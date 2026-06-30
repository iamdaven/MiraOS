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
void fb_draw_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color);
void fb_draw_rect_outline(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color, uint32_t thickness);
void fb_draw_line(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, uint32_t color);
void fb_draw_circle(uint32_t cx, uint32_t cy, uint32_t radius, uint32_t color, bool filled);
void fb_blit(uint32_t dx, uint32_t dy, uint32_t w, uint32_t h, const uint32_t *src, uint32_t src_pitch);
void fb_scroll(uint32_t lines);
void fb_swap_buffers(void);
void fb_clear(uint32_t color);
void fb_shutdown(void);
