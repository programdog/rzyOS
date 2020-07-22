// #include <stdlib.h>
// #include <errno.h>
// #include <string.h>
// #include <sys/stat.h>
// #include <sys/types.h>

#include <stdio.h>
#include "stm32f4xx_usart.h"

/***************************************************************************/

// int _open(const char *name, int flags, int mode)
// {
// 	return -1;
// }

// int _read(int file, char * ptr, int len)
// {
// 	ptr = ptr;
// 	len = len;
// 	errno = EINVAL;
// 	return -1;
// }

/***************************************************************************/

// int _lseek(int file, int ptr, int dir)
// {
// 	file = file;
// 	ptr = ptr;
// 	dir = dir;
// 	return 0;
// }

/***************************************************************************/

int _write(int file, char *ptr, int len)
{
	int index;
	if (!ptr)
	{
		return 0;
	}
	for (index = 0; index < len; index++)
	{
		while (!(USART3->SR & 0x00000040));
		USART_SendData(USART3, ptr[index]);
		// USART3 -> DR = (uint8_t)ptr[index];
	}

	return len;
}

/***************************************************************************/

// int _close(int file)
// {
// 	return 0;
// }

/***************************************************************************/

/* Register name faking - works in collusion with the linker.  */
// register char * stack_ptr asm ("sp");

// caddr_t _sbrk(int incr)
// {
// 	extern char   end asm ("end"); // Defined by the linker.
// 	static char * heap_end;
// 	char *        prev_heap_end;
// 	if (heap_end == NULL)
// 		heap_end = & end;
// 	prev_heap_end = heap_end;
// 	if (heap_end + incr > stack_ptr)
// 	{
// 		// Some of the libstdc++-v3 tests rely upon detecting
// 		// out of memory errors, so do not abort here.
// #if 0
// 		extern void abort (void);
// 		_write (1, "_sbrk: Heap and stack collision\n", 32);
// 		abort ();
// #else
// 		errno = ENOMEM;
// 		return (caddr_t) -1;
// #endif
// 	}
// 	heap_end += incr;
// 	return (caddr_t) prev_heap_end;
// }

// /***************************************************************************/

// int _fstat(int file, struct stat * st)
// {
// 	file = file;
// 	memset (st, 0, sizeof (* st));
// 	st->st_mode = S_IFCHR;
// 	return 0;
// }

// /***************************************************************************/

// int _isatty(int fd)
// {
// 	fd = fd;
// 	return 1;
// }

// int _kill(int pid, int sig)
// {
// 	errno=EINVAL;
// 	return(-1);
// }

// int _getpid(void)
// {
// 	return 1;
// }

/*** EOF ***/
