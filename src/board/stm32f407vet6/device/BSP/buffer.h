#ifndef BUFFER_H
#define BUFFER_H

#include <stdint.h>

#define UART3_RX_BUF_SIZE (256)

uint16_t uart3_rx_buffer_unused(void);

uint16_t uart3_rx_buffer_used(void);

void uart3_rx_buffer_clear(void);

int uart3_rx_buffer_read(uint8_t *c);

void uart3_rx_buffer_write(uint8_t c);



#endif
