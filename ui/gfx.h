#pragma once

#include <stdint.h>

void gfx_init(void);
void gfx_draw_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color);
void gfx_draw_rect_outline(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color, uint32_t thickness);
