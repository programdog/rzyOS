#ifndef RZYOS_MEMORY_H
#define RZYOS_MEMORY_H

#include "rzyOS_event.h"
#include "rzyOS.h"

typedef struct rzyOS_mem_block_s
{
	rzyOS_ecb_s rzyOS_ecb;
	void *mem_start;
	uint32_t bolck_size;
	uint32_t max_count;
	list_t block_list;
} rzyOS_mem_block_s;

void rzyOS_mem_block_init(rzyOS_mem_block_s *rzyOS_mem_block, uint8_t *mem_start, uint32_t bolck_size, uint32_t block_count);

#endif
