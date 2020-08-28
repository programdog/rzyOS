#ifndef RZYOS_PRINTF_H
#define RZYOS_PRINTF_H

#define R_PRINTF_BUFFER_SIZE 256

#ifndef NULL
#define NULL (void *)(0)
#endif

int r_printf(char *fmt, ...);

#endif
