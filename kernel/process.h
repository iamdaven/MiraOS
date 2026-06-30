#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define PROCESS_NAME_MAX 64
#define PROCESS_MAX_OPEN_FILES 16
#define PROCESS_STACK_SIZE 0x100000  /* 1MB stack */
#define PROCESS_MAX_ARGS 64

typedef enum {
    PROCESS_STATE_READY,
    PROCESS_STATE_RUNNING,
    PROCESS_STATE_BLOCKED,
    PROCESS_STATE_ZOMBIE
} process_state_t;

typedef struct process {
    uint32_t pid;
    char name[PROCESS_NAME_MAX];
    process_state_t state;
    
    /* Memory layout */
    uint64_t cr3;
    uint64_t stack_top;
    uint64_t stack_bottom;
    uint64_t entry_point;
    
    /* Context for context switching */
    struct {
        uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
        uint64_t rdi, rsi, rbp, rbx, rdx, rcx, rax;
        uint64_t rip, rflags, rsp;
    } context;
    
    /* Scheduling */
    uint64_t time_slice;
    uint64_t total_time;
    struct process *next;
    struct process *prev;
    
    /* File descriptors */
    int fd_table[PROCESS_MAX_OPEN_FILES];
    
    /* Parent/child relationships */
    struct process *parent;
    struct process *children;
    struct process *next_sibling;
    
    int exit_status;
} process_t;

process_t *process_create(const char *name, uint64_t entry_point);
void process_destroy(process_t *proc);
process_t *process_get_current(void);
void process_set_current(process_t *proc);
void process_switch(process_t *next);
void process_init(void);
uint32_t process_alloc_pid(void);
process_t *process_find_by_pid(uint32_t pid);

/* Scheduler */
void scheduler_init(void);
void scheduler_tick(void);
process_t *scheduler_next(void);
void scheduler_add(process_t *proc);
void scheduler_remove(process_t *proc);