#include "kernel.h"
#include "kernel/panic.h"
#include "kernel/mem.h"
#include "kernel/heap.h"
#include "kernel/syscall.h"
#include "kernel/process.h"
#include "arch/x86_64/gdt.h"
#include "arch/x86_64/idt.h"
#include "arch/x86_64/isr.h"
#include "arch/x86_64/paging.h"
#include "arch/x86_64/cpu.h"
#include "drivers/driver.h"
#include "drivers/framebuffer.h"
#include "drivers/timer.h"
#include "drivers/ata.h"
#include "fs/ramfs.h"
#include "net/net.h"
#include "ui/ui.h"

void kernel_idle(void) {
    uint64_t last_tick = 0;
    for (;;) {
        ui_tick();
        uint64_t t = timer_ticks();
        if (ui_dirty() || t != last_tick) {
            last_tick = t;
            ui_render();
            ui_clear_dirty();
        }
        hlt();
    }
}

void kernel_main(uint64_t magic, uint64_t mbi) {
    (void)magic;
    gdt_init();
    pmm_init(mbi);
    paging_init();
    heap_init();
    idt_init();
    isr_init();
    syscall_init();
    process_init();
    scheduler_init();
    fb_init(mbi);
    driver_subsystem_init();
    ata_init();
    ramfs_init();
    net_init();
    ui_init();
    ui_render();
    sti();
    kernel_idle();
}
