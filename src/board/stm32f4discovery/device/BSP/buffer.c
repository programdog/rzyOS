#include <stdio.h>
#include "buffer.h"

uint8_t uart3_rx_buffer[UART3_RX_BUF_SIZE] = {0};
uint16_t uart3_rx_buffer_write_index = 0;
uint16_t uart3_rx_buffer_read_index = 0;



//buffer未使用量
uint16_t uart3_rx_buffer_unused(void)
{
	return UART3_RX_BUF_SIZE - uart3_rx_buffer_used();
}


//buffer使用量
uint16_t uart3_rx_buffer_used(void)
{
	uint16_t read_index = uart3_rx_buffer_read_index;
	
	if (uart3_rx_buffer_write_index > read_index)
	{
		return (uart3_rx_buffer_write_index - read_index);
	}
	else if (uart3_rx_buffer_write_index < read_index)
	{
		return (UART3_RX_BUF_SIZE - read_index + uart3_rx_buffer_write_index);
	}
	else //uart3_rx_buffer_write_index == read_index
	{
		return 0;
	}
}

void uart3_rx_buffer_clear(void)
{
	uart3_rx_buffer_write_index = 0;
	uart3_rx_buffer_read_index = 0;
}

int uart3_rx_buffer_read(uint8_t *c)
{
	if (uart3_rx_buffer_used())
	{
		*c =  uart3_rx_buffer[uart3_rx_buffer_read_index];
		uart3_rx_buffer_read_index ++;
		uart3_rx_buffer_read_index %= UART3_RX_BUF_SIZE;

		return 1;
	}
	else
	{
		return 0;
	}
}


void uart3_rx_buffer_write(uint8_t c)
{
	if (0 == uart3_rx_buffer_unused())
	{
		uart3_rx_buffer_clear();
	}
	else
	{
		uart3_rx_buffer[uart3_rx_buffer_write_index] = c;
		uart3_rx_buffer_write_index ++;
		uart3_rx_buffer_write_index %= UART3_RX_BUF_SIZE;
	}
}
