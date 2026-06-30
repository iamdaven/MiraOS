#pragma once

#include <stdint.h>
#include <stdbool.h>

#define MEX_MAGIC 0x4D4558
#define MEX_VERSION 1

typedef struct {
    uint32_t magic;
    uint32_t version;
    uint32_t entry_offset;
    uint32_t data_offset;
    uint32_t data_size;
    uint32_t bss_size;
    uint32_t flags;
    char name[64];
} mex_header_t;

typedef struct {
    mex_header_t header;
    uint8_t *code;
    uint8_t *data;
    uint8_t *bss;
    bool loaded;
} mex_app_t;

void app_init(void);
int app_load(const char *path);
int app_run(uint32_t index);
void app_list(void);
uint32_t app_count(void);