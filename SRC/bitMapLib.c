#include "bitMapLib.h"

//初始化位图结构
void bitmap_init(bitmap_s *bitmap)
{
	bitmap -> bitmap_32bit = 0;
}

//位图长度， 目前为32bit
uint32_t bitmap_size(void)
{
	return 32;
}

//按照位置设置位图一个bit
void bitmap_set(bitmap_s *bitmap, uint32_t position)
{
	bitmap -> bitmap_32bit |= 1 << position;
}

//按照位置清空位图一个bit
void bitmap_clean(bitmap_s *bitmap, uint32_t position)
{
	bitmap -> bitmap_32bit &= ~(1 << position);
}

//获取位图第一个bit为1的位置
uint32_t bitmap_get_first_set(bitmap_s *bitmap)
{
	static const uint8_t bitmap_table[] = 
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
	
	//如果在第一个8bit内，则返回第一个8位内位置
	if (bitmap -> bitmap_32bit & 0xff)
	{
		return bitmap_table[bitmap -> bitmap_32bit & 0xff];
	}
	//如果在第二个8bit内，则返回第二个8位内位置
	else if (bitmap -> bitmap_32bit & 0xff00)
	{
		return bitmap_table[(bitmap -> bitmap_32bit >> 8) & 0xff] + 8;
	}
	//如果在第三个8bit内，则返回第三个8位内位置
	else if (bitmap -> bitmap_32bit & 0xff0000)
	{
		return bitmap_table[(bitmap -> bitmap_32bit >> 16) & 0xff] + 16;
	}
	//如果在第四个8bit内，则返回第四个8位内位置
	else if (bitmap -> bitmap_32bit & 0xff000000)
	{
		return bitmap_table[(bitmap -> bitmap_32bit >> 24) & 0xff] + 24;
	}
	//其他情况
	else
	{
		return bitmap_size();
	}
}
