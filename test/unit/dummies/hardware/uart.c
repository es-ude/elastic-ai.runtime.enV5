#include "uart.h"
#include <string.h>

char *uartMessage;
char uartReceive[1000];

bool uart_is_readable(uart_inst_t *uart) { return *uartMessage != 0; }

char uart_getc(uart_inst_t *uart) {
    char ret = *uartMessage;
    uartMessage++;
    return ret;
}

uint uart_init(uart_inst_t *uart, uint baudrate) {
    uartReceive[0] = 0;
    return 0;
}

int uart_set_baudrate(uart_inst_t *uart, uint baudrate) { return 1; }

void uart_set_hw_flow(uart_inst_t *uart, bool cts, bool rts) {}

void uart_set_format(uart_inst_t *uart, uint data_bits, uint stop_bits,
                     uart_parity_t parity) {}

void uart_set_fifo_enabled(uart_inst_t *uart, bool enabled) {}

void (*interruptHandler)();

void irq_set_exclusive_handler(uint num, void (*handler)()) {
    interruptHandler = handler;
    interruptHandler();
}

void irq_set_enabled(uint num, bool enabled) {}

void uart_set_irq_enables(uart_inst_t *uart, bool rx_has_data,
                          bool tx_needs_data) {}

void InvokeIrqHandler() {
    interruptHandler();
    if (!uart_is_readable(NULL))
        uartMessage++;
}

void uart_puts(uart_inst_t *uart, const char *s) {
    if (strnlen(s, 4) == 2 && strstr(s, "\r\n") != NULL) {
        strcpy(uartReceive, s);
        InvokeIrqHandler();
    }
}
