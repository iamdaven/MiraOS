#pragma once

#include <stdint.h>
#include <stdbool.h>

void shell_init(void);
void shell_tick(void);
void shell_render(void);
bool shell_dirty(void);
void shell_clear_dirty(void);
