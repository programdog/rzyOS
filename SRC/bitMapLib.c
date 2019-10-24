#include "bitMapLib.h"

void tBitMapInit(tBitMap *bitmap)
{
	bitmap -> bitMap = 0;
}

uint32_t tBitMapSize(void)
{
	return 32;
}

void tBitMapSet(tBitMap *bitmap, uint32_t pos)
{
	bitmap -> bitMap |= 1 << pos;
}

void tBitMapClean(tBitMap *bitmap, uint32_t pos)
{
	bitmap -> bitMap &= ~(1 << pos);
}
uint32_t tBitMapGetFirstSet(tBitMap *bitmap)
{
	static const uint8_t bitMapTable[] = 
	{
		/* 00 */ 0xff, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, // oxff means no bit is 1
		/* 10 */ 4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
		/* 20 */ 5,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
		/* 30 */ 4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
		/* 40 */ 6,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
		/* 50 */ 4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
		/* 60 */ 5,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
		/* 70 */ 4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
		/* 80 */ 7,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
		/* 90 */ 4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
		/* A0 */ 5,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
		/* B0 */ 4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
		/* C0 */ 6,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
		/* D0 */ 4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
		/* E0 */ 5,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
		/* F0 */ 4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0
	};
	
	if (bitmap -> bitMap & 0xff)
	{
		return bitMapTable[bitmap -> bitMap & 0xff];
	}
	else if (bitmap -> bitMap & 0xff00)
	{
		return bitMapTable[(bitmap -> bitMap >> 8) & 0xff] + 8;
	}
	else if (bitmap -> bitMap & 0xff0000)
	{
		return bitMapTable[(bitmap -> bitMap >> 16) & 0xff] + 16;
	}
	else if (bitmap -> bitMap & 0xff000000)
	{
		return bitMapTable[(bitmap -> bitMap >> 24) & 0xff] + 24;
	}
	else
	{
		return tBitMapSize();
	}
}
