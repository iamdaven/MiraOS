#pragma once

#include <stdint.h>

void timer_init(void);
uint64_t timer_ticks(void);
void timer_on_irq(void);
