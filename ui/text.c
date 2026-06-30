#include "text.h"
#include "font.h"
#include "drivers/framebuffer.h"

void text_init(void) {
}

void text_draw_char(uint32_t x, uint32_t y, char c, uint32_t fg, uint32_t bg) {
    if (c < 32 || c > 127)
        return;
    const uint8_t *glyph = font8x16[c - 32];
    for (int row = 0; row < 16; row++) {
        uint8_t bits = glyph[row];
        for (int col = 0; col < 8; col++) {
            uint32_t color = (bits & (0x80 >> col)) ? fg : bg;
            fb_put_pixel(x + col, y + row, color);
        }
    }
}

void text_draw(uint32_t x, uint32_t y, const char *str, uint32_t fg, uint32_t bg) {
    uint32_t cx = x;
    uint32_t cy = y;
    for (int i = 0; str[i]; i++) {
        if (str[i] == '\n') {
            cx = x;
            cy += 16;
            continue;
        }
        text_draw_char(cx, cy, str[i], fg, bg);
        cx += 8;
    }
}
