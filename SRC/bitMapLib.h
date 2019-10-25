#ifndef __BITMAPLIB_H
#define __BITMAPLIB_H

#include <stdint.h>

typedef struct bitmap_s
{
	uint32_t bitmap_32bit;
} bitmap_s;

void bitmap_init(bitmap_s *bitmap);
uint32_t bitmap_size(void);
void bitmap_set(bitmap_s *bitmap, uint32_t position);
void bitmap_clean(bitmap_s *bitmap, uint32_t position);
uint32_t bitmap_get_first_set(bitmap_s *bitmap);

#endif
