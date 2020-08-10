#include "rzyOS_std.h"

extern int _write(int file, char *ptr, int len);

int stdout_write(struct file *f, const void *buf, size_t size)
{
	return _write(1, (char *)buf, size);
}

int stdin_init(void)
{
	return 0;
}

int stdout_init(void)
{
	file_operations_s ops = {0};
	ops.write = stdout_write;
	return rzyOS_fs_register_dev("/dev/stdout", ops);
}

int stderr_init(void)
{
	return 0;
}

int std_init(void)
{
	stdin_init();
	stdout_init();
	stderr_init();

	return 0;
}
