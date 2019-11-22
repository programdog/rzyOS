#include "bitMapLib.h"

#define INLIST_FIRST_NODE head_node.next_node		//	headnode --> first
#define INLIST_LAST_NODE head_node.pre_node			//	last  <-- headnode

//init pre_node and next_node of node_t to point itself
void node_init(node_t *node)
{
	node -> pre_node = node;
	node -> next_node = node;
}

//init pre_node and next_node of the list head_node to point itself 
void list_init(list_t *list)
{
	list -> INLIST_FIRST_NODE = &(list -> head_node);
	list -> INLIST_LAST_NODE = &(list -> head_node);
	list -> node_counter = 0;
}

//return how many node of this list
uint32_t list_count(list_t *list)
{
	return list -> node_counter;
}

//return the first node address
//(head)->[first]->(node)->(node)->(node)->[last]->(head)
node_t *list_first_node(list_t *list)
{
	node_t *node = (node_t *)0;
	
	if (list -> node_counter != 0)
	{
		node = list -> head_node.next_node;
	}
	
	return node;
}

//return the last node address
node_t *list_last_node(list_t *list)
{
	node_t *node = (node_t *)0;
	
	if (list -> node_counter != 0)
	{
		node = list -> head_node.pre_node;
	}
	
	return node;
}

//return address of the target node's pre_node
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

//return address of the target node's next_node
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

// break all notes , then make next_node and pre_node of the struct node_t point itself
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

//insert a node to list at the first position
//(head)->[first]->(node)->(node)->(node)->[last]->(head)
void list_add_first(list_t *list, node_t *node)
{
	node -> pre_node = list -> INLIST_FIRST_NODE -> pre_node;
	node -> next_node = list -> INLIST_FIRST_NODE;
	
	list -> INLIST_FIRST_NODE -> pre_node = node;
	list -> INLIST_FIRST_NODE = node;
	list -> node_counter ++;
}

//insert a node to list at the last position
//(head)->[first]->(node)->(node)->(node)->[last]->(head)
void list_add_last(list_t *list, node_t *node)
{
	node -> next_node = &(list -> head_node);
	node -> pre_node = list -> INLIST_LAST_NODE;
	
	list -> INLIST_LAST_NODE -> next_node = node;
	list -> INLIST_LAST_NODE = node;
	list -> node_counter ++;
}

//remove the first position node
//(head)->[first]->(node)->(node)->(node)->[last]->(head)
//return address which node to be remove
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

//insert a node after the appoint node
void list_insert_node_after(list_t *list, node_t *node_after, node_t *node_to_insert)
{
	node_to_insert -> pre_node = node_after;
	node_to_insert -> next_node = node_after -> next_node;
	
	node_after -> next_node = node_to_insert;
	node_after -> next_node -> pre_node = node_to_insert;
	
	list -> node_counter ++;
}

//remove the appoint node
void list_remove_pos_node(list_t *list, node_t *node)
{
	node -> pre_node -> next_node = node -> next_node;
	node -> next_node -> pre_node = node -> pre_node;
	
	list -> node_counter --;
}
