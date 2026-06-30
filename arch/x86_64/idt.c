#include "idt.h"
#include "cpu.h"

static struct idt_entry idt[IDT_ENTRIES];
static struct idt_ptr idt_pointer;

void idt_set_gate(uint8_t num, uint64_t handler, uint8_t type_attr) {
    idt[num].offset_low = handler & 0xFFFF;
    idt[num].selector = 0x08;
    idt[num].ist = 0;
    idt[num].type_attr = type_attr;
    idt[num].offset_mid = (handler >> 16) & 0xFFFF;
    idt[num].offset_high = (handler >> 32) & 0xFFFFFFFF;
    idt[num].zero = 0;
}

void idt_init(void) {
    for (int i = 0; i < IDT_ENTRIES; i++)
        idt_set_gate(i, 0, 0);

    idt_pointer.limit = sizeof(idt) - 1;
    idt_pointer.base = (uint64_t)&idt;
    lidt_desc(idt_pointer.limit, idt_pointer.base);
}
