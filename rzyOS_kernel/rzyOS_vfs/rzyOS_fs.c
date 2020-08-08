#include "rzyOS_fs.h"


//注册设备节点
int rzyOS_fs_register_dev(char *path, file_operations_s ops)
{
	return vfs_insert_node(path, ops);
}


//找到设备节点
vfs_node_s *rzyOS_fs_get_node(char *path)
{
	return vfs_find_node(path);
}
