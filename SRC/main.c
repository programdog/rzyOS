#define NVIC_INT_CTRL	0xe000ed04
#define	NVIC_PENDSVSET	0X10000000
#define	NVIC_SYSPRI2	0xe000ed22
#define	NVIC_PENDSV_PRI	0x000000ff

#define MEM32(addr)	*(volatile unsigned long *)(addr)
#define MEM8(addr)	*(volatile unsigned char *)(addr)

void triggerPendSV(void)
{
	MEM8(NVIC_SYSPRI2) = NVIC_PENDSV_PRI;
	MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;
}

typedef struct Blocktype_t
{
	unsigned long *stackptr;
} Blocktype_t;

Blocktype_t *blockPtr; 

void delay(int count)
{
	while(--count > 0);
}

int flag;
unsigned long stack_bufffer[1024];
Blocktype_t block;

int main()
{
	block.stackptr = &stack_bufffer[1024];
	blockPtr = &block;
	for (;;)
	{
		flag = 0;
		delay(100);
		flag = 1;
		delay(100);
		
		triggerPendSV();
	}
}
