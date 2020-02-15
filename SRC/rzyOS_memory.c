#include "rzyOS_memory.h"


void rzyOS_mem_block_init(rzyOS_mem_block_s *rzyOS_mem_block, uint8_t *mem_start, uint32_t bolck_size, uint32_t block_count)
{
	uint8_t *mem_block_start = (uint8_t *)mem_start;
	uint8_t *mem_block_end = mem_block_start + bolck_size * block_count;

	if (bolck_size < sizeof(node_t))
	{
		return ;
	}

	rzyOS_event_init(&(rzyOS_mem_block -> rzyOS_ecb), event_type_mem_block);

	rzyOS_mem_block -> mem_start = mem_block_start;
	rzyOS_mem_block -> bolck_size = bolck_size;
	rzyOS_mem_block -> max_count = block_count;

	list_init(&(rzyOS_mem_block -> block_list));

	while (mem_block_start < mem_block_end)
	{
		node_init((node_t *)mem_block_start);

		list_add_last(&(rzyOS_mem_block -> block_list), (node_t *)mem_block_start);

		mem_block_start += bolck_size;
	}
}
