#include "bitMapLib.h"

void node_init(node_t *node)
{
	node -> pre_node = node;
	node -> next_node = node;
}

#define INLIST_FIRST_NODE head_node.next_node		//	headnode --> first
#define INLIST_LAST_NODE head_node.pre_node			//	last  <-- headnode

void list_init(list_t *list)
{
	list -> INLIST_FIRST_NODE = &(list -> head_node);
	list -> INLIST_LAST_NODE = &(list -> head_node);
	list -> node_counter = 0;
}

uint32_t list_count(list_t *list)
{
	return list -> node_counter;
}

node_t *list_first_node(list_t *list)
{
	node_t *node = (node_t *)0;
	
	if (list -> node_counter != 0)
	{
		node = list -> head_node.next_node;
	}
	
	return node;
}

node_t *list_last_node(list_t *list)
{
	node_t *node = (node_t *)0;
	
	if (list -> node_counter != 0)
	{
		node = list -> head_node.pre_node;
	}
	
	return node;
}

node_t *list_pos_node_pre(list_t *list, node_t *node)
{
	if (node -> pre_node == node)
	{
		return (node_t *)0;
	}
	else
	{
		return node -> pre_node;	
	}
}

node_t *list_pos_node_next(list_t *list, node_t *node)
{
	if (node -> next_node == node)
	{
		return (node_t *)0;
	}
	else
	{
		return node -> next_node;	
	}
}

void list_remode_all(list_t *list)
{
	uint32_t count;
	node_t *next_node_temp;
	
	next_node_temp = list -> head_node.next_node;
	for (count = list -> node_counter; count > 0; count --)
	{
		node_t *current_node = next_node_temp;
		next_node_temp = current_node -> next_node;
		
		current_node -> next_node = current_node;
		current_node -> pre_node = current_node;
	}
	
	list -> INLIST_FIRST_NODE = &(list ->head_node);
	list -> INLIST_FIRST_NODE = &(list ->head_node);
	list -> node_counter = 0;
}

void list_add_first(list_t *list, node_t *node)
{
	node -> pre_node = list -> INLIST_FIRST_NODE -> pre_node;
	node -> next_node = list -> INLIST_FIRST_NODE;
	
	list -> INLIST_FIRST_NODE -> pre_node = node;
	list -> INLIST_FIRST_NODE = node;
	list -> node_counter ++;
}

void list_add_last(list_t *list, node_t *node)
{
	node -> next_node = &(list -> head_node);
	node -> pre_node = list -> INLIST_LAST_NODE;
	
	list -> INLIST_LAST_NODE -> next_node = node;
	list -> INLIST_LAST_NODE = node;
	list -> node_counter ++;
}

node_t *remove_list_first(list_t *list)
{
	node_t *node = (node_t *)0;
	
	if (list -> node_counter != 0)
	{
		node = list -> INLIST_FIRST_NODE;
		
		node -> next_node -> pre_node = &(list -> head_node);
		list -> INLIST_FIRST_NODE = node -> next_node;
		list -> node_counter --;
	}
	return node;
}

void list_insert_node_after(list_t *list, node_t *node_after, node_t *node_to_insert)
{
	node_to_insert -> pre_node = node_after;
	node_to_insert -> next_node = node_after -> next_node;
	
	node_after -> next_node = node_to_insert;
	node_after -> next_node -> pre_node = node_to_insert;
	
	list -> node_counter ++;
}

void list_remove_pos_node(list_t *list, node_t *node)
{
	node -> pre_node -> next_node = node -> next_node;
	node -> next_node -> pre_node = node -> pre_node;
	
	list -> node_counter --;
}
