#ifndef RZYOS_CMD_LIST_H
#define RZYOS_CMD_LIST_H

#include "stdio.h"

//命令元素
typedef struct rzyOS_cmd_s
{
	//命令字符串表
	char const *cmd_string;
	//最大支持参数个数
	uint32_t max_args;
	//命令回调函数
	void (*handle)(int argc, void *argv);
	//命令使用帮助
	char *help;
} rzyOS_cmd_s;



#endif
