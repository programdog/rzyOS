#ifndef RZYOS_FS_H
#define RZYOS_FS_H

#include "rzyOS_vfs.h"

int rzyOS_fs_register_dev(char *path, file_operations_s ops);

vfs_node_s *rzyOS_fs_get_node(char *path);

#endif
