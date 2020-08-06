#include "rzyOS_fs.h"


int rzyOS_fs_register_dev(char *path, file_operations_s ops)
{
	return vfs_insert_node(path, ops);
}


vfs_node_s *rzyOS_fs_get_node(char *path)
{
	return vfs_find_node(path);
}
