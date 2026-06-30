#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define INPUT_MAX 256

void input_init(void);
void input_poll(void);
const char *input_buffer(void);
size_t input_length(void);
void input_clear(void);
bool input_dirty(void);
void input_clear_dirty(void);
