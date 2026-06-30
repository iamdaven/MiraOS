#pragma once

#include <stdint.h>

#define SYSCALL_EXIT   0
#define SYSCALL_READ   1
#define SYSCALL_WRITE  2
#define SYSCALL_OPEN   3
#define SYSCALL_CLOSE  4
#define SYSCALL_DRAW_RECT 5
#define SYSCALL_DRAW_TEXT 6
#define SYSCALL_GET_TICKS 7

void syscall_init(void);
int64_t syscall_dispatch(uint64_t num, uint64_t a1, uint64_t a2, uint64_t a3, uint64_t a4, uint64_t a5);
