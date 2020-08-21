#include "buffer.h"

uint8_t uart3_rx_buffer[UART3_RX_BUF_SIZE] = {0};
uint8_t uart3_rx_buffer_head = 0;
uint8_t uart3_rx_buffer_tail = 0;

uint8_t uart3_rx_buffer_available()
{
	
}


uint8_t uart3_rx_buffer_used()
{
	uint8_t tail_count = uart3_rx_buffer_tail;
	
	if (uart3_rx_buffer_head >= tail_count)
	{
		return (uart3_rx_buffer_head - tail_count);
	}
	else //uart3_rx_buffer_head < tail_count
	{
		return (UART3_RX_BUF_SIZE - tail_count + uart3_rx_buffer_head);
	}
}