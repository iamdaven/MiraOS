#include "app.h"
#include "fs/vfs.h"
#include "kernel/heap.h"

#define MAX_APPS 16

static mex_app_t apps[MAX_APPS];
static uint32_t num_apps;

void app_init(void) {
    num_apps = 0;
}

int app_load(const char *path) {
    if (num_apps >= MAX_APPS)
        return -1;
    
    int fd = vfs_open(path, VFS_O_RDONLY);
    if (fd < 0)
        return -1;
    
    mex_header_t header;
    ssize_t n = vfs_read(fd, &header, sizeof(header));
    if (n != sizeof(header)) {
        vfs_close(fd);
        return -1;
    }
    
    if (header.magic != MEX_MAGIC) {
        vfs_close(fd);
        return -1;
    }
    
    uint32_t idx = num_apps++;
    apps[idx].header = header;
    apps[idx].loaded = false;
    
    vfs_close(fd);
    return idx;
}

int app_run(uint32_t index) {
    if (index >= num_apps)
        return -1;
    
    mex_app_t *app = &apps[index];
    
    if (!app->loaded) {
        int fd = vfs_open(app->header.name, VFS_O_RDONLY);
        if (fd < 0)
            return -1;
        
        vfs_lseek(fd, app->header.data_offset, VFS_SEEK_SET);
        
        app->code = (uint8_t *)kmalloc(app->header.entry_offset + app->header.data_size + app->header.bss_size);
        if (!app->code) {
            vfs_close(fd);
            return -1;
        }
        
        vfs_read(fd, app->code, app->header.entry_offset + app->header.data_size);
        vfs_close(fd);
        
        app->data = app->code + app->header.data_offset;
        app->bss = app->data + app->header.data_size;
        for (uint32_t i = 0; i < app->header.bss_size; i++)
            app->bss[i] = 0;
        
        app->loaded = true;
    }
    
    void (*entry)(void) = (void (*)(void))(app->code + app->header.entry_offset);
    entry();
    
    return 0;
}

void app_list(void) {
    for (uint32_t i = 0; i < num_apps; i++) {
        (void)apps[i];
    }
}

uint32_t app_count(void) {
    return num_apps;
}