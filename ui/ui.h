#pragma once

#include <stdbool.h>

void ui_init(void);
void ui_tick(void);
void ui_render(void);
bool ui_dirty(void);
void ui_clear_dirty(void);
