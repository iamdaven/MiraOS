#include "process.h"
#include "kernel.h"
#include "kernel/mem.h"
#include "kernel/heap.h"
#include "kernel/panic.h"
#include "lib/mem.h"
#include "lib/ds.h"
#include "arch/x86_64/cpu.h"
#include "arch/x86_64/paging.h"

static process_t *current_process = 0;
static process_t *process_list = 0;
static uint32_t next_pid = 1;
static uint64_t kernel_stack_top;

process_t *process_create(const char *name, uint64_t entry_point) {
    process_t *proc = (process_t *)kmalloc(sizeof(process_t));
    if (!proc)
        return 0;
    
    memset(proc, 0, sizeof(process_t));
    
    proc->pid = process_alloc_pid();
    ds_strcpy(proc->name, name);
    proc->state = PROCESS_STATE_READY;
    proc->entry_point = entry_point;
    
    /* Allocate stack */
    void *stack = pmm_alloc_page();
    if (!stack) {
        kfree(proc);
        return 0;
    }
    proc->stack_bottom = (uint64_t)stack;
    proc->stack_top = proc->stack_bottom + PROCESS_STACK_SIZE;
    
    /* Initialize context */
    memset(&proc->context, 0, sizeof(proc->context));
    proc->context.rsp = proc->stack_top;
    proc->context.rip = entry_point;
    proc->context.rflags = 0x202;  /* Enable interrupts */
    
    /* Add to process list */
    proc->next = process_list;
    if (process_list)
        process_list->prev = proc;
    process_list = proc;
    
    return proc;
}

void process_destroy(process_t *proc) {
    if (!proc)
        return;
    
    /* Free stack */
    if (proc->stack_bottom)
        pmm_free_page((void *)proc->stack_bottom);
    
    /* Remove from process list */
    if (proc->prev)
        proc->prev->next = proc->next;
    if (proc->next)
        proc->next->prev = proc->prev;
    if (process_list == proc)
        process_list = proc->next;
    
    kfree(proc);
}

process_t *process_get_current(void) {
    return current_process;
}

void process_set_current(process_t *proc) {
    current_process = proc;
}

void process_switch(process_t *next) {
    if (current_process == next)
        return;
    
    /* Save current context */
    if (current_process) {
        __asm__ volatile (
            "movq %%rsp, %0\n"
            "movq %%rbp, %1\n"
            : "=r"(current_process->context.rsp),
              "=r"(current_process->context.rbp)
            : 
            : "memory"
        );
    }
    
    current_process = next;
    
    /* Restore next context */
    __asm__ volatile (
        "movq %0, %%rsp\n"
        "movq %1, %%rbp\n"
        : 
        : "r"(next->context.rsp),
          "r"(next->context.rbp)
        : "memory", "rsp", "rbp"
    );
}

void process_init(void) {
    kernel_stack_top = 0;
    current_process = 0;
    process_list = 0;
    next_pid = 1;
}

uint32_t process_alloc_pid(void) {
    return next_pid++;
}

process_t *process_find_by_pid(uint32_t pid) {
    process_t *proc = process_list;
    while (proc) {
        if (proc->pid == pid)
            return proc;
        proc = proc->next;
    }
    return 0;
}

/* Simple round-robin scheduler */
static process_t *ready_queue = 0;
static process_t *ready_queue_tail = 0;

void scheduler_init(void) {
    ready_queue = 0;
    ready_queue_tail = 0;
}

void scheduler_tick(void) {
    /* Called on timer interrupt */
    if (!ready_queue)
        return;
    
    /* Move current process to back of queue */
    if (ready_queue->next) {
        ready_queue_tail->next = ready_queue;
        ready_queue_tail = ready_queue;
        ready_queue = ready_queue->next;
        ready_queue_tail->next = 0;
    }
}

process_t *scheduler_next(void) {
    return ready_queue;
}

void scheduler_add(process_t *proc) {
    if (!proc)
        return;
    
    proc->state = PROCESS_STATE_READY;
    
    if (!ready_queue) {
        ready_queue = proc;
        ready_queue_tail = proc;
        proc->next = 0;
        proc->prev = 0;
    } else {
        ready_queue_tail->next = proc;
        proc->prev = ready_queue_tail;
        proc->next = 0;
        ready_queue_tail = proc;
    }
}

void scheduler_remove(process_t *proc) {
    if (!proc)
        return;
    
    if (proc->prev)
        proc->prev->next = proc->next;
    else
        ready_queue = proc->next;
    
    if (proc->next)
        proc->next->prev = proc->prev;
    else
        ready_queue_tail = proc->prev;
    
    proc->next = 0;
    proc->prev = 0;
}