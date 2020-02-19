#include "rzyOS_memory.h"

//内存管理初始化函数
//parameter : 
//rzyOS_mem_block_s *rzyOS_mem_block ： 内存管理结构
//uint8_t *mem_start ： 内存块起始地址
//uint32_t bolck_size ： 子内存块容量大小
//uint32_t block_count ： 子内存块个数
void rzyOS_mem_block_init(rzyOS_mem_block_s *rzyOS_mem_block, uint8_t *mem_start, uint32_t bolck_size, uint32_t block_count)
{
	//记录起始地址与结束地址
	uint8_t *mem_block_start = (uint8_t *)mem_start;
	uint8_t *mem_block_end = mem_block_start + bolck_size * block_count;

	//因为每个子内存块要在子内存块的初始位置建立链接节点， 所以子内存块的最小size必须大于node_t
	if (bolck_size < sizeof(node_t))
	{
		return ;
	}

	//事件结构初始化
	rzyOS_event_init(&(rzyOS_mem_block -> rzyOS_ecb), event_type_mem_block);

	//内存管理结构赋值
	rzyOS_mem_block -> mem_start = mem_block_start;
	rzyOS_mem_block -> bolck_size = bolck_size;
	rzyOS_mem_block -> max_count = block_count;

	//初始化内存管理结构链表
	list_init(&(rzyOS_mem_block -> block_list));

	//遍历初始化子内存块节点，并加入链表
	while (mem_block_start < mem_block_end)
	{
		//初始化子内存块节点
		node_init((node_t *)mem_block_start);
		//把子内存块加入到子内存块管理链表
		list_add_last(&(rzyOS_mem_block -> block_list), (node_t *)mem_block_start);
		//遍历地址向后偏移
		mem_block_start += bolck_size;
	}
}
