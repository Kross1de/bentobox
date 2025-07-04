#pragma once
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define MAX_PATH 256

typedef enum vfs_node_type {
    VFS_NONE,
    VFS_FILE,
    VFS_DIRECTORY,
    VFS_CHARDEVICE,
    VFS_BLOCKDEVICE
} vfs_node_type_t;

typedef struct vfs_node {
    char name[MAX_PATH];
    bool open;
    enum vfs_node_type type;
    size_t size;
    uint16_t perms;
    uint32_t inode;
    struct vfs_node *parent;
    struct vfs_node *children;
    struct vfs_node *next;
    long(*read)(struct vfs_node *node, void *buffer, long offset, size_t len);
    long(*write)(struct vfs_node *node, void *buffer, long offset, size_t len);
} vfs_node_t;

extern struct vfs_node *vfs_root;

void vfs_install(void);
void vfs_add_node(struct vfs_node *parent, struct vfs_node *node);
void vfs_add_device(struct vfs_node *node);
void vfs_resolve_path(char *s, struct vfs_node *node);
long vfs_read(struct vfs_node *node, void *buffer, long offset, size_t len);
long vfs_write(struct vfs_node *node, void *buffer, long offset, size_t len);
struct vfs_node *vfs_create_node(const char *name, enum vfs_node_type type);
struct vfs_node *vfs_open(struct vfs_node *current, const char *path);
int vfs_close(struct vfs_node *node);