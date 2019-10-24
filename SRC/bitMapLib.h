#ifndef __BITMAPLIB_H
#define __BITMAPLIB_H

#include <stdint.h>

typedef struct tBitMap
{
	uint32_t bitMap;
} tBitMap;

void tBitMapInit(tBitMap *bitmap);
uint32_t tBitMapSize(void);
void tBitMapSet(tBitMap *bitmap, uint32_t pos);
void tBitMapClean(tBitMap *bitmap, uint32_t pos);
uint32_t tBitMapGetFirstSet(tBitMap *bitmap);

#endif
