#include "rzyOS_fctl.h"
#include "rzyOS_fs.h"
#include "rzyOS_schedule.h"

uint32_t fcntl_first_empty(task_tcb_s *task_tcb);

int fcntl_alloc(task_tcb_s *task_tcb, uint32_t index);

//0为已经使用， 1为空闲可用
static const uint8_t bitmap_table[] = 
{
	/* 00 */ 0,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	/* 10 */ 4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	/* 20 */ 5,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	/* 30 */ 4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	/* 40 */ 6,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	/* 50 */ 4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	/* 60 */ 5,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	/* 70 */ 4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	/* 80 */ 7,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	/* 90 */ 4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	/* A0 */ 5,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	/* B0 */ 4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	/* C0 */ 6,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	/* D0 */ 4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	/* E0 */ 5,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	/* F0 */ 4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0
};



int open(char *path, int oflag, int mode)
{
	vfs_node_s *node = rzyOS_fs_get_node(path);

	if (node == NULL)
	{
		return -1;
	}

	task_tcb_s *task_tcb = get_current_tcb();

	if (node -> ops.open == NULL)
	{
		return -1;
	}

	node -> ops.open(NULL);

	uint32_t index = fcntl_first_empty(task_tcb);

	if (index >= RZYOS_FS_NODE_NUM)
	{
		return -1;
	}

	//申请节点
	fcntl_alloc(task_tcb, index);

	task_tcb -> fs_nodes[index] = node;

	return index;
}


//0为已经使用， 1为空闲可用
//找到第一个空闲位置的索引号
uint32_t fcntl_first_empty(task_tcb_s *task_tcb)
{
	//如果在第一个8bit内，则返回第一个8位内位置
	if (task_tcb -> fs_node_map & 0xff)
	{
		return bitmap_table[task_tcb -> fs_node_map & 0xff];
	}
	//如果在第二个8bit内，则返回第二个8位内位置
	else if (task_tcb -> fs_node_map & 0xff00)
	{
		return bitmap_table[(task_tcb -> fs_node_map >> 8) & 0xff] + 8;
	}
	//如果在第三个8bit内，则返回第三个8位内位置
	else if (task_tcb -> fs_node_map & 0xff0000)
	{
		return bitmap_table[(task_tcb -> fs_node_map >> 16) & 0xff] + 16;
	}
	//如果在第四个8bit内，则返回第四个8位内位置
	else if (task_tcb -> fs_node_map & 0xff000000)
	{
		return bitmap_table[(task_tcb -> fs_node_map >> 24) & 0xff] + 24;
	}
	//其他情况
	else
	{
		return bitmap_size();
	}
}


//根据索引号分配节点
int fcntl_alloc(task_tcb_s *task_tcb, uint32_t index)
{
	if (task_tcb == NULL)
	{
		return -1;
	}

	//将使用位图中指定的索引号置为0,表示此位置占用
	task_tcb -> fs_node_map &= ~(1 << index);

	return 0;
}
