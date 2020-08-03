#ifndef RZYOS_VFS_H
#define RZYOS_VFS_H

#include "rzyOS_semaphore.h"

#define NODE_NAME_SIZE (32)

struct file
{
//reserve
};

struct inode
{
//reserve
};

typedef struct file_operations_s
{
	int (*open)(struct file *filep);
	int (*close)(struct file *filep);
	uint32_t (*read)(struct file *filep, char *buffer, uint32_t buflen);
	uint32_t (*write)(struct file *filep, const char *buffer, uint32_t buflen);
	// uint32_t (*seek)(struct file *filep, uint32_t offset, int whence);
	int (*ioctl)(struct file *filep, int cmd, unsigned long arg);
}file_operations_s;

typedef struct vfs_node_s
{
	struct vfs_node_s *brother;
	struct vfs_node_s *child;
	char name[NODE_NAME_SIZE];
	struct file_operations_s ops;
} vfs_node_s;

typedef struct vfs_root_mangement_s
{
	vfs_node_s *root_node;
	rzyOS_sem_s sem_rw;
} vfs_root_mangement_s;

int vfs_init(void);

int vfs_insert_node(char *path, file_operations_s ops);

vfs_node_s *vfs_find_node(char *path);

#endif
