#if RZYOS_ENABLE_MEMORY == 1

#include "rzyOS_mm1.h"
#include "rzyOS_semaphore.h"


//堆内存静态分配
static uint8_t memory[HEAP_SIZE] = {0};

static block_s *list_head = NULL;

static rzyOS_sem_s mm1_protect_sem;



void *my_malloc(uint32_t size);
void split(block_s *src_block, uint32_t size);
void merge(void);
void my_free(void* addr);



void *malloc(uint32_t size)
{
	rzyOS_sem_wait(&mm1_protect_sem, 0);

	void *addr = my_malloc(size);

	rzyOS_sem_post(&mm1_protect_sem);

	return addr;
}

void free(void *addr)
{
	rzyOS_sem_wait(&mm1_protect_sem, 0);

	my_free(addr);

	rzyOS_sem_post(&mm1_protect_sem);
}

void rzyOS_memory_mm1_init(void)
{
	//获取heap地址
	list_head = (void *)memory;
	//可用空间大小
	list_head -> free_size = HEAP_SIZE - sizeof(block_s);
	//1：空闲 0：使用
	list_head -> free = 1;
	//指向空
	list_head -> next = NULL;


	rzyOS_sem_init(&mm1_protect_sem, 1, 1);
}

void *my_malloc(uint32_t size)
{
	block_s *cursor;

	void *result = (void *)0;


	if (!(list_head -> free_size))
	{
		return NULL;
	}

	cursor = list_head;

	//从头部遍历
	//当前内存块被使用 || 当前内存块空间小于需要的空间
	//&&下一个内存块不是末尾（即当前内存块已经为最后一个）
	while ((((cursor -> free_size) < size) || ((cursor -> free) == 0)) && (cursor -> next != NULL))
	{
		//切换下一个内存块
		cursor = cursor -> next;
	}

	//大小正好
	if ((cursor -> free_size) == size)
	{
		cursor -> free = 0;
		//使用的位置
		result = (void*)(++ cursor);

		return result;
	}
	//剩余空间大小 大于 所需要的内存大小
	else if ((cursor -> free_size) > (size + sizeof(block_s)))
	{
		//分割区块函数
		split(cursor, size);
		//使用的位置
		result = (void*)(++ cursor);

		return result;
	}
	//分配失败
	else
	{
		result = NULL;

		return result;
	}
}


void split(block_s *src_block, uint32_t size)
{
	//定义分配后剩下的内存块指针new_block
	block_s *new_block = (block_s *)((uint8_t *)src_block + sizeof(block_s) + size);
	//定义new_block的size大小
	new_block -> free_size = (src_block -> free_size) - size - sizeof(block_s);
	//设置为空闲状态
	new_block -> free = 1;
	//连接
	new_block -> next = src_block -> next;


	src_block -> free_size = size;
	src_block -> free = 0;
	//连接
	src_block -> next = new_block;
}


void merge(void)
{
	block_s *cursor;

	cursor = list_head;

	//头部开始遍历
	//下一个有节点，未到末尾，继续检查（当前非末尾）
	while ((cursor -> next) != NULL)
	{
		//当前为空闲 && 下一个也是空闲
		//可以merge
		if ((cursor -> free) && (cursor -> next -> free))
		{
			//当前的free_size与下一个合并
			cursor -> free_size += (cursor -> next -> free_size) + sizeof(block_s);
			//连接
			cursor -> next = cursor -> next -> next;
		}
		//检查下一个
		cursor = cursor -> next;
	}
}


void my_free(void* addr)
{
	//地址处于heap中
	if (((void *)memory <= addr) && (addr <= (void *)(memory + HEAP_SIZE)))
	{
		block_s *cursor = addr;

		//获取块地址
		-- cursor;
		//置位
		cursor -> free = 1;
		//merge检查
		merge();
	}
}

#endif
