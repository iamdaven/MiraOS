#include "gfx.h"
#include "drivers/framebuffer.h"

void gfx_init(void) {
}

void gfx_draw_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color) {
    fb_fill_rect(x, y, w, h, color);
}

void gfx_draw_rect_outline(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color, uint32_t thickness) {
    for (uint32_t t = 0; t < thickness; t++) {
        fb_fill_rect(x + t, y + t, w - 2 * t, 1, color);
        fb_fill_rect(x + t, y + h - 1 - t, w - 2 * t, 1, color);
        fb_fill_rect(x + t, y + t, 1, h - 2 * t, color);
        fb_fill_rect(x + w - 1 - t, y + t, 1, h - 2 * t, color);
    }
}
