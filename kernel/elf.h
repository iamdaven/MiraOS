#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define ELF_MAGIC 0x464C457FU  /* "\x7FELF" */

typedef struct {
    uint32_t magic;
    uint8_t arch;
    uint8_t endian;
    uint8_t version;
    uint8_t os_abi;
    uint8_t padding[8];
    uint16_t type;
    uint16_t machine;
    uint32_t version2;
    uint64_t entry;
    uint64_t phoff;
    uint64_t shoff;
    uint32_t flags;
    uint16_t ehsize;
    uint16_t phentsize;
    uint16_t phnum;
    uint16_t shentsize;
    uint16_t shnum;
    uint16_t shstrndx;
} elf_header_t;

typedef struct {
    uint32_t type;
    uint32_t flags;
    uint64_t offset;
    uint64_t vaddr;
    uint64_t paddr;
    uint64_t filesz;
    uint64_t memsz;
    uint64_t align;
} elf_program_header_t;

#define PT_LOAD 1
#define PF_X 1
#define PF_W 2
#define PF_R 4

bool elf_load(const void *data, size_t size, uint64_t *entry_point);