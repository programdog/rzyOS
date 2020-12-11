#ifndef RZYOS_RSH_H
#define RZYOS_RSH_H

#include <stdio.h>
#include "rzyOSconfig.h"
#include "rzyOS_event.h"

#define ARGV_NUM 4

#define CMD_CHAR_LEN 20

#define CMD_BUF_SIZE 50

typedef struct rzyOS_cmd_analyze_s
{
	char receive_buf[CMD_BUF_SIZE];
	char processed_buf[CMD_BUF_SIZE];
	int32_t cmd_argv[ARGV_NUM];
} rzyOS_cmd_analyze_s;

//命令元素
typedef struct rzyOS_cmd_s
{
	//命令字符串表
	char const *cmd_list;
	//最大支持参数个数
	uint32_t max_args;
	//命令回调函数
	void (*handle)(int argc, void *argv);
	//命令使用帮助
	char *help;
} rzyOS_cmd_s;


void rzyOS_rzh_show(void);

void rzyOS_rsh(void *param);

void rzyOS_rsh_task_init(void);

#endif
