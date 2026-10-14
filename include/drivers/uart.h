#ifndef UART_H
#define UART_H

#include <stdint.h>

void UART_init(void);

uint8_t UART_rxByte(void);

void UART_txByte(uint8_t byte);

#endif