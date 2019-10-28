#ifndef __BITMAPLIB_H
#define __BITMAPLIB_H

#include <stdint.h>

typedef struct bitmap_s
{
	uint32_t bitmap_32bit;
} bitmap_s;

void bitmap_init(bitmap_s *bitmap);
uint32_t bitmap_size(void);
void bitmap_set(bitmap_s *bitmap, uint32_t position);
void bitmap_clean(bitmap_s *bitmap, uint32_t position);
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
