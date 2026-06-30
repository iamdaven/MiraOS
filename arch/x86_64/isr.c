#include "isr.h"
#include "idt.h"
#include "cpu.h"
#include "drivers/driver.h"
#include "drivers/pic.h"
#include "kernel/panic.h"

static isr_handler_t handlers[ISR_COUNT];

void pic_remap(void) {
    outb(0x20, 0x11);
    outb(0xA0, 0x11);
    outb(0x21, 0x20);
    outb(0xA1, 0x28);
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    outb(0x21, 0x0);
    outb(0xA1, 0x0);
}

void isr_register_handler(uint8_t num, isr_handler_t handler) {
    handlers[num] = handler;
}

static void (*const isr_stubs[])(void) = {
    isr0, isr1, isr2, isr3, isr4, isr5, isr6, isr7,
    isr8, isr9, isr10, isr11, isr12, isr13, isr14, isr15,
    isr16, isr17, isr18, isr19, isr20, isr21, isr22, isr23,
    isr24, isr25, isr26, isr27, isr28, isr29, isr30, isr31,
    isr32, isr33, isr34, isr35, isr36, isr37, isr38, isr39,
    isr40, isr41, isr42, isr43, isr44, isr45, isr46, isr47
};

void isr_init(void) {
    pic_remap();
    for (int i = 0; i < 48; i++)
        idt_set_gate(i, (uint64_t)isr_stubs[i], 0x8E);
}

void isr_handler(struct registers *regs) {
    if (regs->int_no < 32) {
        if (regs->int_no == 14) {
            panic("page fault");
        }
        panic("cpu exception");
    }

    if (handlers[regs->int_no])
        handlers[regs->int_no](regs);
    else if (regs->int_no >= 32 && regs->int_no < 48)
        driver_dispatch_irq(regs->int_no - 32);

    if (regs->int_no >= 32 && regs->int_no < 48)
        drivers_pic_send_eoi(regs->int_no - 32);
}
