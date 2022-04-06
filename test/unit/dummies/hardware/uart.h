//
// Created by Lukas on 03.02.2022.
//

#ifndef SENSOR_BOARD_UART_H
#define SENSOR_BOARD_UART_H

#include "string.h"
#include "stdbool.h"

typedef struct uart_inst uart_inst_t;
typedef unsigned int uint;
typedef enum {
    UART_PARITY_NONE,
    UART_PARITY_EVEN,
    UART_PARITY_ODD
} uart_parity_t;

#define uart0 NULL
#define uart1 NULL

extern char *uartMessage;
extern char uartReceive[1000];

bool uart_is_readable(uart_inst_t *uart);

char uart_getc(uart_inst_t *uart);

uint uart_init(uart_inst_t *uart, uint baudrate);

int uart_set_baudrate(uart_inst_t *uart, uint baudrate);

void uart_set_hw_flow(uart_inst_t *uart, bool cts, bool rts);

void uart_set_format(uart_inst_t *uart, uint data_bits, uint stop_bits, uart_parity_t parity);

void uart_set_fifo_enabled(uart_inst_t *uart, bool enabled);

void irq_set_exclusive_handler(uint num, void (*handler)());

void irq_set_enabled(uint num, bool enabled);

void uart_set_irq_enables(uart_inst_t *uart, bool rx_has_data, bool tx_needs_data);

void uart_puts(uart_inst_t *uart, const char *s);

#endif //SENSOR_BOARD_UART_H
