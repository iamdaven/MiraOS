#pragma once

#include <stdint.h>

void text_init(void);
void text_draw(uint32_t x, uint32_t y, const char *str, uint32_t fg, uint32_t bg);
void text_draw_char(uint32_t x, uint32_t y, char c, uint32_t fg, uint32_t bg);
