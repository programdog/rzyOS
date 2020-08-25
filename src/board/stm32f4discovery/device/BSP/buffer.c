#include "buffer.h"

uint8_t uart3_rx_buffer[UART3_RX_BUF_SIZE] = {0};
uint8_t uart3_rx_buffer_head = 0;
uint8_t uart3_rx_buffer_tail = 0;


uint8_t uart3_rx_buffer_unused(void);
uint8_t uart3_rx_buffer_used(void);
void uart3_rx_buffer_clear(void);


//buffer未使用量
uint8_t uart3_rx_buffer_unused(void)
{
	return (UART3_RX_BUF_SIZE - uart3_rx_buffer_used());
}


//buffer使用量
uint8_t uart3_rx_buffer_used(void)
{
	uint8_t tail_index = uart3_rx_buffer_tail;
	
	if (uart3_rx_buffer_head > tail_index)
	{
		return (uart3_rx_buffer_head - tail_index);
	}
	else if (uart3_rx_buffer_head < tail_index)
	{
		return (UART3_RX_BUF_SIZE - tail_index + uart3_rx_buffer_head);
	}
	else //uart3_rx_buffer_head == tail_index
	{
		return 0;
	}
}

void uart3_rx_buffer_clear(void)
{
	uart3_rx_buffer_head = 0;
	uart3_rx_buffer_tail = 0;
}

int uart3_rx_buffer_read(uint8_t *c)
{
	if (uart3_rx_buffer_used())
	{
		*c =  uart3_rx_buffer[uart3_rx_buffer_tail];
		uart3_rx_buffer_tail ++;
		uart3_rx_buffer_tail %= UART3_RX_BUF_SIZE;
		return 1;
	}
	else
	{
		return 0;
	}
}


void uart3_rx_buffer_write(uint8_t c)
{
	if (uart3_rx_buffer_unused())
	{
		uart3_rx_buffer_clear();
	}
	else
	{
		uart3_rx_buffer[uart3_rx_buffer_head] = c;
		uart3_rx_buffer_head ++;
		uart3_rx_buffer_head %= UART3_RX_BUF_SIZE;
	}
}
