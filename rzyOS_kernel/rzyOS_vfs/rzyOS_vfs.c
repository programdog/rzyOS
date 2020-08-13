#include <string.h>
#include "rzyOS_mm1.h"
#include "rzyOS_vfs.h"


//定义根节点管理结构指针
static vfs_root_mangement_s *vfs_root_mangement = NULL;
//定义根节点管理结构
static vfs_root_mangement_s vfs_root_mangement_instance;
//定义根节点
static vfs_node_s vfs_node_root;


static int vfs_insert_node_r(vfs_node_s **node, char *abs_path, file_operations_s ops);

static vfs_node_s *vfs_find_node_r(vfs_node_s *node, char *abs_path);

static vfs_node_s *vfs_find_node_in_brother(vfs_node_s *node, char *node_name);

//设备树root节点初始化
int vfs_init(void)
{
	vfs_root_mangement = &vfs_root_mangement_instance;
	rzyOS_sem_init(&(vfs_root_mangement -> sem_rw), 1, 1);
	vfs_root_mangement -> root_node = &vfs_node_root;
	//创建根节点/
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

//插入节点
//rzyOS_fs_register_dev()调用
int vfs_insert_node(char *path, file_operations_s ops)
{
	//判断地址头
	if (path[0] != '/')
	{
		return -1;
	}

	rzyOS_sem_wait(&vfs_root_mangement -> sem_rw, 0);
	int ret = vfs_insert_node_r(&vfs_root_mangement -> root_node -> child, &path[1], ops);
	rzyOS_sem_post(&vfs_root_mangement -> sem_rw);

	return ret;
}

//parameter :
//vfs_node_s **node : 
//char *abs_path : 
//file_operations_s ops : 
//return 0 :success ; -1 : false
int vfs_insert_node_r(vfs_node_s **node, char *abs_path, file_operations_s ops)
{
	if (node == NULL)
	{
		return -1;
	}
	if (abs_path == NULL)
	{
		return -1;
	}
	if (abs_path[0] == 0)
	{
		return -1;
	}

	char node_name[NODE_NAME_SIZE] = {0};

	char *p = abs_path;

	//‘/’与‘/’之间的第一个字段赋值
	for (int i = 0; (*p != '/') && (*p != 0) && (i < NODE_NAME_SIZE); i ++)
	{
		node_name[i] = *p++;
	}

	if (*p == '/' && *p != 0)
	{
		p++;
	}

	//已经存在节点
	while (*node != NULL)
	{
		//判断是否为同名
		if (strcmp((*node) -> name, node_name) == 0)
		{
			//无节点，则创造子节点
			return vfs_insert_node_r(&(*node) -> child, p, ops);
		}
		//有节点，则切换指向同级节点
		node = &(*node) -> brother;
	}

	//为新节点申请空间
	vfs_node_s *node_new = malloc(sizeof(vfs_node_s));

	//填充节点名字
	strncpy(node_new -> name, node_name, NODE_NAME_SIZE);

	node_new -> child = NULL;
	node_new -> brother = NULL;

	//初始化设备节点操作结构
	memset(&node_new -> ops, 0, sizeof(file_operations_s));

	*node = node_new;

	//p非0,还有节点字段
	//为新的父节点时， 则创造子节点
	if (*p != 0)
	{
		return vfs_insert_node_r(&node_new -> child, p, ops);
	}

	//设备节点操作结构赋值
	memcpy(&node_new -> ops, &ops, sizeof(file_operations_s));

	return 0;
}

//查找节点
//rzyOS_fs_get_node()调用
vfs_node_s *vfs_find_node(char *path)
{
	if (path[0] != '/')
	{
		return NULL;
	}

	if (path[1] == 0)
	{
		return vfs_root_mangement -> root_node;
	}

	rzyOS_sem_wait(&vfs_root_mangement -> sem_rw, 0);
	vfs_node_s *node = vfs_find_node_r(vfs_root_mangement -> root_node -> child, &path[1]);
	rzyOS_sem_post(&vfs_root_mangement -> sem_rw);

	return node;
}


vfs_node_s *vfs_find_node_r(vfs_node_s *node, char *abs_path)
{
	if (node == NULL)
	{
		return NULL;
	}
	if (abs_path == NULL)
	{
		return NULL;
	}
	if (abs_path[0] == 0)
	{
		return NULL;
	}

	char node_name[NODE_NAME_SIZE] = {0};

	char *p = abs_path;
	for (int i = 0; *p != '/' && *p != 0 && i < NODE_NAME_SIZE; i++)
	{
		node_name[i] = *p++;
	}

	vfs_node_s *ret = vfs_find_node_in_brother(node, node_name);
	if (*p == '/')
	{
		p++;
		return vfs_find_node_r(ret -> child, p);
	}
	return ret;
}

//查找同级节点
vfs_node_s *vfs_find_node_in_brother(vfs_node_s *node, char *node_name)
{
	while (node != NULL)
	{
		//判断是否为当前节点
		if (strcmp(node -> name, node_name) == 0)
		{
			return node;
		}
		//
		node = node -> brother;
	}
	return NULL;
}
