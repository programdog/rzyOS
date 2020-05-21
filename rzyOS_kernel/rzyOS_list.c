#include "rzyOS_bitmap.h"

#define INLIST_FIRST_NODE head_node.next_node		//	headnode --> first
#define INLIST_LAST_NODE head_node.pre_node			//	last  <-- headnode

//初始化前节点与后节点的指针指向自己
//init pre_node and next_node of node_t to point itself
void node_init(node_t *node)
{
	node -> pre_node = node;
	node -> next_node = node;
}

//初始化头节点指针指向头节点自己
//init pre_node and next_node of the list head_node to point itself 
void list_init(list_t *list)
{
	list -> INLIST_FIRST_NODE = &(list -> head_node);
	list -> INLIST_LAST_NODE = &(list -> head_node);
	list -> node_counter = 0;
}

//返回链表中的节点个数
//return how many node of this list
uint32_t list_count(list_t *list)
{
	return list -> node_counter;
}

//返回链表中第一个节点的地址
//return the first node address
//(head)->[first]->(node)->(node)->(node)->[last]->(head)
node_t *list_first_node(list_t *list)
{
	node_t *node = (node_t *)0;
	
	//若计数不为0,则返回第一个节点地址
	if (list -> node_counter != 0)
	{
		node = list -> head_node.next_node;
	}
	
	return node;
}

//返回链表中最后一个节点的地址
//return the last node address
node_t *list_last_node(list_t *list)
{
	node_t *node = (node_t *)0;
	
	//若计数不为0,则返回最后一个节点地址
	if (list -> node_counter != 0)
	{
		node = list -> head_node.pre_node;
	}
	
	return node;
}

//返回链表中目标节点的前一个节点地址
//return address of the target node's pre_node
node_t *list_pos_node_pre(node_t *node)
{
	//若前一个节点是指向自己，说明没有节点，则返回null
	if (node -> pre_node == node)
	{
		return (node_t *)0;
	}
	else
	{
		return node -> pre_node;
	}
}

//返回链表中目标节点的后一个节点地址
//return address of the target node's next_node
node_t *list_pos_node_next(node_t *node)
{
	//若后一个节点是指向自己，说明没有节点，则返回null
	if (node -> next_node == node)
	{
		return (node_t *)0;
	}
	else
	{
		return node -> next_node;
	}
}

//删除所有节点
//断开所有的节点， 并让所有的节点中前节点和后节点的地址指向自己，
// break all notes , then make next_node and pre_node of the struct node_t point itself
void list_remode_all(list_t *list)
{
	uint32_t count;
	node_t *next_node_temp;
	
	//获取第一个节点
	next_node_temp = list -> head_node.next_node;
	//按照count数循环
	for (count = list -> node_counter; count > 0; count --)
	{
		node_t *current_node = next_node_temp;
		next_node_temp = current_node -> next_node;
		
		//初始化当前节点，指向自己
		current_node -> next_node = current_node;
		current_node -> pre_node = current_node;
	}
	
	//初始化头节点，指向自己
	list -> INLIST_FIRST_NODE = &(list ->head_node);
	list -> INLIST_FIRST_NODE = &(list ->head_node);
	list -> node_counter = 0;
}

//在第一个位置插入节点
//insert a node to list at the first position
//(head)->[first]->(node)->(node)->(node)->[last]->(head)
void list_add_first(list_t *list, node_t *node)
{
	//把原第一个节点内部的前指向赋值给当前第一个节点内部的前指向
	node -> pre_node = list -> INLIST_FIRST_NODE -> pre_node;
	//把原第一个节点地址赋值给当前第一个节点内部的下一个指向
	node -> next_node = list -> INLIST_FIRST_NODE;
	
	//配置列表头部管理节点的数据
	list -> INLIST_FIRST_NODE -> pre_node = node;
	list -> INLIST_FIRST_NODE = node;
	list -> node_counter ++;
}

//在在最后的位置插入节点
//insert a node to list at the last position
//(head)->[first]->(node)->(node)->(node)->[last]->(head)
void list_add_last(list_t *list, node_t *node)
{
	//配置插入节点的内部数据
	node -> next_node = &(list -> head_node);
	node -> pre_node = list -> INLIST_LAST_NODE;
	
	//配置列表头部管理节点的数据
	list -> INLIST_LAST_NODE -> next_node = node;
	list -> INLIST_LAST_NODE = node;
	list -> node_counter ++;
}

//删除第一个节点，并返回删除的节点指针
//remove the first position node
//(head)->[first]->(node)->(node)->(node)->[last]->(head)
//return address which node to be remove
node_t *remove_list_first(list_t *list)
{
	node_t *node = (node_t *)0;
	
	//如果了链表统计有节点
	if (list -> node_counter != 0)
	{
		//获取链表的第一个节点
		node = list -> INLIST_FIRST_NODE;
		
		//重新分配被删除第一个节点后边的节点的内部地址
		node -> next_node -> pre_node = &(list -> head_node);
		//改变头节点对第一个节点的指向
		list -> INLIST_FIRST_NODE = node -> next_node;
		list -> node_counter --;
	}
	return node;
}

//在指定的节点之后插入一个节点
//insert a node after the appoint node
void list_insert_node_after(list_t *list, node_t *node_after, node_t *node_to_insert)
{
	//被插入的节点的内部地址赋值
	node_to_insert -> pre_node = node_after;
	node_to_insert -> next_node = node_after -> next_node;
	
	//指定位置的节点前后节点内部赋值
	node_after -> next_node = node_to_insert;
	node_after -> next_node -> pre_node = node_to_insert;
	
	//数量统计++
	list -> node_counter ++;
}

//删除指定的节点
//remove the appoint node
void list_remove_pos_node(list_t *list, node_t *node)
{
	//被删除的节点的前后节点的内部赋值
	node -> pre_node -> next_node = node -> next_node;
	node -> next_node -> pre_node = node -> pre_node;
	
	//数量统计--
	list -> node_counter --;
}
