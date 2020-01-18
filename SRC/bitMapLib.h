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



typedef struct node_t
{
	struct node_t *pre_node;
	struct node_t *next_node;
} node_t;

typedef struct list_t
{
	node_t head_node;
	uint32_t node_counter;
} list_t;

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
