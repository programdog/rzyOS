#ifndef RZYOS_FS_H
#define RZYOS_FS_H

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
	uint32_t (*seek)(struct file *filep, uint32_t offset, int whence);
	int (*ioctl)(struct file *filep, int cmd, unsigned long arg);
}file_operations_s;

typedef struct vfs_node_s
{
	struct vfs_node_s *brother;
	struct vfs_node_s *child;
	char name[NODE_NAME_SIZE];
	struct file_operations_s ops;
} vfs_node_s;

#endif
