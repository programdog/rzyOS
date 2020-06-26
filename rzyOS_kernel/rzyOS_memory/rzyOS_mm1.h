#ifndef RZYOS_MM1_H
#define RZYOS_MM1_H

#include <stdint.h>
#include <stdio.h>
#include "rzyOSconfig.h"


//内存块结构
typedef struct block_s
{
	uint16_t free_size;//区块大小
	uint16_t free;//是否已使用(1未使用)
	struct block_s *next;//指向下一个区块
} block_s;

//heap : 5k
#define HEAP_SIZE (MM1_HEAP_SIZE)



void *malloc(uint32_t size);

void free(void *addr);

void rzyOS_memory_mm1_init(void);

#endif
