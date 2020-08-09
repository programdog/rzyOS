#include "rzyOS_std.h"

extern int _write(int file, char *ptr, int len);

int stdout_write(struct file *f, const void *buf, size_t size)
{
	char *p = (char *)buf;

	for (size_t i = 0; i < size; i++)
	{
		//
	}
	return size;
}

int stdout_init(void)
{
	file_operations_s ops = {0};
	ops.write = stdout_write;
	return rzyOS_fs_register_dev("/dev/stdout", ops);
}

