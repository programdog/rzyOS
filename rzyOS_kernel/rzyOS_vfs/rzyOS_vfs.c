#include <string.h>
#include "rzyOS_vfs.h"


static vfs_root_mangement_s *vfs_root_mangement = NULL;
static vfs_root_mangement_s vfs_root_mangement_instance;

static vfs_node_s vfs_node_root;


int vfs_init(void)
{
	vfs_root_mangement = &vfs_root_mangement_instance;
	rzyOS_sem_init(&(vfs_root_mangement -> sem_rw), 1, 1);
	vfs_root_mangement -> root_node = &vfs_node_root;
	strncpy(vfs_root_mangement -> root_node -> name, "/", NODE_NAME_SIZE);
	
	vfs_root_mangement -> root_node -> brother = NULL;
	vfs_root_mangement -> root_node -> child = NULL;
	vfs_root_mangement -> root_node -> ops.open = NULL;
	vfs_root_mangement -> root_node -> ops.close = NULL;
	vfs_root_mangement -> root_node -> ops.read = NULL;
	vfs_root_mangement -> root_node -> ops.write = NULL;
	vfs_root_mangement -> root_node -> ops.ioctl = NULL;

	return 0;
}


