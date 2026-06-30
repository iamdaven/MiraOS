#include "gdt.h"
#include "cpu.h"

static struct gdt_entry gdt[GDT_ENTRIES];
static struct gdt_ptr gdt_pointer;

static struct {
    uint32_t reserved0;
    uint64_t rsp0;
    uint64_t rsp1;
    uint64_t rsp2;
    uint64_t reserved1;
    uint64_t ist1;
    uint64_t ist2;
    uint64_t ist3;
    uint64_t ist4;
    uint64_t ist5;
    uint64_t ist6;
    uint64_t ist7;
    uint64_t reserved2;
    uint16_t reserved3;
    uint16_t iopb_offset;
} __attribute__((packed)) tss;

static void gdt_set_entry(int idx, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) {
    gdt[idx].limit_low = limit & 0xFFFF;
    gdt[idx].base_low = base & 0xFFFF;
    gdt[idx].base_mid = (base >> 16) & 0xFF;
    gdt[idx].access = access;
    gdt[idx].granularity = (limit >> 16) & 0x0F;
    gdt[idx].granularity |= gran & 0xF0;
    gdt[idx].base_high = (base >> 24) & 0xFF;
}

static void gdt_set_tss(int idx, uint64_t base, uint32_t limit) {
    struct {
        uint16_t limit_low;
        uint16_t base_low;
        uint8_t base_mid;
        uint8_t access;
        uint8_t granularity;
        uint8_t base_high;
        uint32_t base_upper;
        uint32_t reserved;
    } __attribute__((packed)) *tss_desc = (void *)&gdt[idx];

    tss_desc->limit_low = limit & 0xFFFF;
    tss_desc->base_low = base & 0xFFFF;
    tss_desc->base_mid = (base >> 16) & 0xFF;
    tss_desc->access = 0x89;
    tss_desc->granularity = (limit >> 16) & 0x0F;
    tss_desc->base_high = (base >> 24) & 0xFF;
    tss_desc->base_upper = (uint32_t)(base >> 32);
    tss_desc->reserved = 0;
}

void gdt_init(void) {
    gdt_set_entry(0, 0, 0, 0, 0);
    gdt_set_entry(1, 0, 0xFFFFF, 0x9A, 0xA0);
    gdt_set_entry(2, 0, 0xFFFFF, 0x92, 0xA0);
    gdt_set_entry(3, 0, 0xFFFFF, 0xFA, 0xA0);
    gdt_set_entry(4, 0, 0xFFFFF, 0xF2, 0xA0);

    tss.iopb_offset = sizeof(tss);
    gdt_set_tss(5, (uint64_t)&tss, sizeof(tss) - 1);

    gdt_pointer.limit = sizeof(gdt) - 1;
    gdt_pointer.base = (uint64_t)&gdt;

    lgdt_desc(gdt_pointer.limit, gdt_pointer.base);

    __asm__ volatile (
        "mov $0x10, %%ax\n"
        "mov %%ax, %%ds\n"
        "mov %%ax, %%es\n"
        "mov %%ax, %%fs\n"
        "mov %%ax, %%gs\n"
        "mov %%ax, %%ss\n"
        "push $0x08\n"
        "lea 1f(%%rip), %%rax\n"
        "push %%rax\n"
        "lretq\n"
        "1:\n"
        ::: "rax", "memory"
    );

    load_tss(GDT_TSS);
}

uint16_t gdt_user_code_selector(void) { return GDT_USER_CODE | 3; }
uint16_t gdt_user_data_selector(void) { return GDT_USER_DATA | 3; }
uint16_t gdt_tss_selector(void) { return GDT_TSS; }
