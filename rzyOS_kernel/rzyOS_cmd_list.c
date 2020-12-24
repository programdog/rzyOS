#include "rzyOS_cmd_list.h"


void printf_hello(int argc, void *argv)
{
	printf("HelloWorld!\r\n");
}

//命令表
const rzyOS_cmd_s rzyOS_cmd_list[] =
{
	/*命令		参数数目		处理函数			帮助信息	*/
	{"hello",	0,		printf_hello,		"hello -print HelloWorld!"},
//	{"arg",		8,		handle_arg,			"arg<arg1> <arg2> ... -for testing, print input argument"},
};
