#pragma once

#include <stddef.h>
#include <stdint.h>

typedef long ssize_t;

#define VFS_MAX_OPEN 16
#define VFS_O_RDONLY 0
#define VFS_O_WRONLY 1
#define VFS_O_RDWR   2
#define VFS_O_CREAT  4

#define VFS_SEEK_SET 0
#define VFS_SEEK_CUR 1
#define VFS_SEEK_END 2

typedef struct vfs_node vfs_node_t;

typedef struct {
    int (*open)(vfs_node_t *node, int flags);
    int (*close)(vfs_node_t *node);
    ssize_t (*read)(vfs_node_t *node, void *buf, size_t count, size_t offset);
    ssize_t (*write)(vfs_node_t *node, const void *buf, size_t count, size_t offset);
} vfs_ops_t;

struct vfs_node {
    char name[64];
    uint32_t type;
    size_t size;
    void *data;
    vfs_ops_t *ops;
    vfs_node_t *parent;
    vfs_node_t *children;
    vfs_node_t *next_sibling;
    int child_count;
};

#define VFS_TYPE_FILE 1
#define VFS_TYPE_DIR  2

void vfs_init(vfs_node_t *root);
vfs_node_t *vfs_root(void);
vfs_node_t *vfs_lookup(const char *path);
int vfs_open(const char *path, int flags);
int vfs_close(int fd);
ssize_t vfs_read(int fd, void *buf, size_t count);
ssize_t vfs_write(int fd, const void *buf, size_t count);
int vfs_lseek(int fd, size_t offset, int whence);
vfs_node_t *vfs_create_file(const char *path, const void *data, size_t size);
