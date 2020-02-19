#ifndef RZYOS_MEMORY_H
#define RZYOS_MEMORY_H

#include "rzyOS_event.h"
#include "rzyOS.h"

typedef struct rzyOS_mem_block_s
{
	//任务控制块
	rzyOS_ecb_s rzyOS_ecb;
	//内存起始地址
	void *mem_start;
	//子内存块大小
	uint32_t bolck_size;
	//子内存块个数
	uint32_t max_count;
	//子内存块管理链表
	list_t block_list;
} rzyOS_mem_block_s;

//内存管理初始化函数
void rzyOS_mem_block_init(rzyOS_mem_block_s *rzyOS_mem_block, uint8_t *mem_start, uint32_t bolck_size, uint32_t block_count);

#endif
