#ifndef __BITMAPLIB_H
#define __BITMAPLIB_H

#include <stdint.h>

//位图结构
typedef struct bitmap_s
{
	uint32_t bitmap_32bit;
} bitmap_s;

//初始化位图结构
void bitmap_init(bitmap_s *bitmap);

//位图长度， 目前为32bit
uint32_t bitmap_size(void);

//按照位置设置位图一个bit
void bitmap_set(bitmap_s *bitmap, uint32_t position);

//按照位置清空位图一个bit
void bitmap_clean(bitmap_s *bitmap, uint32_t position);

//获取位图第一个bit为1的位置
uint32_t bitmap_get_first_set(bitmap_s *bitmap);


//节点结构
typedef struct node_t
{
	//前节点地址
	struct node_t *pre_node;
	//后节点地址
	struct node_t *next_node;
} node_t;


//双向链表管理结构
typedef struct list_t
{
	//首节点
	node_t head_node;
	//节点计数
	uint32_t node_counter;
} list_t;

//返回父结构（parent）的地址
//parameter ： 
//node ： 节点地址
//parent ： 父结构类型（我们这里用任务TCB）
//name ： 节点类型（比如是延时节点还是就绪节点或其他节点）
#define node_parent(node, parent, name) (parent *)((uint32_t)node - (uint32_t)&((parent *)0) -> name)

void node_init(node_t *node);

void list_init(list_t *list);

uint32_t list_count(list_t *list);

node_t *list_first_node(list_t *list);

node_t *list_last_node(list_t *list);

node_t *list_pos_node_pre(list_t *list, node_t *node);

node_t *list_pos_node_next(list_t *list, node_t *node);

void list_remode_all(list_t *list);

void list_add_first(list_t *list, node_t *node);

void list_add_last(list_t *list, node_t *node);

node_t *remove_list_first(list_t *list);

void list_insert_node_after(list_t *list, node_t *node_after, node_t *node_to_insert);

void list_remove_pos_node(list_t *list, node_t *node);

#endif
