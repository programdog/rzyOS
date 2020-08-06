#ifndef RZYOS_VFS_H
#define RZYOS_VFS_H

#include "rzyOS_semaphore.h"

//节点名字长度
#define NODE_NAME_SIZE (32)

struct file
{
//reserve
};

struct inode
{
//reserve
};

//posix标准操作函数
typedef struct file_operations_s
{
	int (*open)(struct file *filep);
	int (*close)(struct file *filep);
	uint32_t (*read)(struct file *filep, char *buffer, uint32_t buflen);
	uint32_t (*write)(struct file *filep, const char *buffer, uint32_t buflen);
	// uint32_t (*seek)(struct file *filep, uint32_t offset, int whence);
	int (*ioctl)(struct file *filep, int cmd, unsigned long arg);
}file_operations_s;

//设备节点结构
typedef struct vfs_node_s
{
	struct vfs_node_s *brother;
	struct vfs_node_s *child;
	char name[NODE_NAME_SIZE];
	struct file_operations_s ops;
} vfs_node_s;

//设备树根节点管理结构
typedef struct vfs_root_mangement_s
{
	vfs_node_s *root_node;
	rzyOS_sem_s sem_rw;
} vfs_root_mangement_s;


//设备树root节点初始化
int vfs_init(void);

//插入节点
int vfs_insert_node(char *path, file_operations_s ops);

//查找节点
vfs_node_s *vfs_find_node(char *path);

#endif
