#ifndef RZYOS_RSH_H
#define RZYOS_RSH_H

#include <stdio.h>
#include "rzyOSconfig.h"
#include "rzyOS_event.h"
#include "rzyOS_cmd_list.h"

//uart.h -> 涉及串口打印， board 相关！！！
#include "uart.h"

//参数个数
#define ARGV_NUM 4

//命令字符支持的最长长度
#define CMD_CHAR_LEN 20

//命令行支持的总长度
#define CMD_BUF_SIZE 50

//命令解析结构体
typedef struct rzyOS_cmd_analyze_s
{
	//接收到的命令行buffer
	char receive_buf[CMD_BUF_SIZE];
	//处理后命令行buffer
	char processed_buf[CMD_BUF_SIZE];
	//参数数组
	int32_t cmd_argv[ARGV_NUM];
} rzyOS_cmd_analyze_s;


void rzyOS_rsh_show(void);

void rzyOS_rsh(void *param);

void rzyOS_rsh_task_init(void);

#endif
