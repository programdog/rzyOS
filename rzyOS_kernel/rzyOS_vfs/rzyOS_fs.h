#ifndef RZYOS_FS_H
#define RZYOS_FS_H

#include "rzyOS_vfs.h"

//注册设备节点
int rzyOS_fs_register_dev(char *path, file_operations_s ops);

//找到设备节点
vfs_node_s *rzyOS_fs_get_node(char *path);

#endif
