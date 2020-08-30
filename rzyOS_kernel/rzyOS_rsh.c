#include "rzyOS_rsh.h"

static char rzh_show_1[300] = "\r\n\r\n                 ____   _____ \r\n                / __ \\ / ____|\r\n _ __ _____   _| |  | | (___  \r\n| '__|_  / | | | |  | |\\___ \\ \r\n| |   / /| |_| | |__| |____) | \r\n|_|  /___|\\__, |\\____/|_____/ \r\n           __/ |              \r\n          |___/               \r\n\r\n";

static char copyright_1[] = "";

void rzyOS_rzh_show(void)
{
	printf("%s", rzh_show_1);
}
