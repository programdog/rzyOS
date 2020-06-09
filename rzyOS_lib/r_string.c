#include "r_string.h"

void r_memset(void *mem, int val, unsigned long size)
{
	char *p = (char *)mem;
	char v = (char)val;
	for (unsigned long i = 0; i < size; i++)
	{
		p[i] = v;
	}
}
