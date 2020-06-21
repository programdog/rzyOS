#ifndef RZYOS_MM2_H
#define RZYOS_MM2_H

#include "rzyOS_event.h"
#include "rzyOS_schedule.h"

//内存块管理结构体
typedef struct rzyOS_mem_block_s
{
	//事件控制块
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

//内存块信息结构体
typedef struct rzyOS_mem_info_s
{
	//当前使用的存储块计数
	uint32_t mem_count;
	//子内存块总个数
	uint32_t max_count;
	//子内存块大小
	uint32_t block_size;
	//当前等待存储块的任务数量
	uint32_t task_count;
} rzyOS_mem_info_s;

//内存管理初始化函数
void rzyOS_mem_block_init(rzyOS_mem_block_s *rzyOS_mem_block, uint8_t *mem_start, uint32_t bolck_size, uint32_t block_count);

//任务获取内存块函数(阻塞)
uint32_t rzyOS_mem_block_wait(rzyOS_mem_block_s *rzyOS_mem_block, uint8_t **mem, uint32_t wait_time);

//任务获取内存块函数(非阻塞)
uint32_t rzyOS_mem_block_no_wait(rzyOS_mem_block_s *rzyOS_mem_block, uint8_t **mem);

//子内存块释放函数
void rzyOS_mem_block_post(rzyOS_mem_block_s *rzyOS_mem_block, uint8_t **mem);

//内存块销毁函数
uint32_t rzyOS_mem_block_destroy(rzyOS_mem_block_s *rzyOS_mem_block);

//内存块信息获取函数
void rzyOS_mem_block_get_info(rzyOS_mem_block_s *rzyOS_mem_block, rzyOS_mem_info_s *rzyOS_mem_info);

#endif
