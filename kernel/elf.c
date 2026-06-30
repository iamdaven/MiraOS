#include "elf.h"
#include "kernel.h"
#include "kernel/mem.h"
#include "kernel/heap.h"
#include "kernel/panic.h"
#include "lib/mem.h"
#include "arch/x86_64/paging.h"

bool elf_load(const void *data, size_t size, uint64_t *entry_point) {
    (void)size;
    const elf_header_t *header = (const elf_header_t *)data;
    
    /* Verify ELF magic */
    if (header->magic != ELF_MAGIC)
        return false;
    
    /* Check for x86_64 */
    if (header->machine != 0x3E)
        return false;
    
    /* Set entry point */
    if (entry_point)
        *entry_point = header->entry;
    
    /* Load program segments */
    const uint8_t *ptr = (const uint8_t *)data;
    for (int i = 0; i < header->phnum; i++) {
        const elf_program_header_t *ph = (const elf_program_header_t *)(ptr + header->phoff + i * header->phentsize);
        
        if (ph->type != PT_LOAD)
            continue;
        
        /* Map segment into memory */
        uint64_t vaddr = ph->vaddr;
        uint64_t offset = ph->offset;
        size_t filesz = ph->filesz;
        size_t memsz = ph->memsz;
        
        /* Allocate and copy pages */
        for (size_t j = 0; j < memsz; j += 4096) {
            void *page = pmm_alloc_page();
            if (!page)
                return false;
            
            size_t copy_size = (j + 4096 <= filesz) ? 4096 : (filesz > j ? filesz - j : 0);
            if (j < filesz) {
                const uint8_t *src = ptr + offset + j;
                uint8_t *dst = (uint8_t *)page;
                for (size_t k = 0; k < copy_size && (j + k) < memsz; k++)
                    dst[k] = src[k];
            }
            
            paging_map_page(vaddr + j, (uint64_t)page, PAGE_PRESENT | PAGE_WRITE);
        }
    }
    
    return true;
}