#ifndef UART_TO_ESP_INTERNAL_HEADER
#define UART_TO_ESP_INTERNAL_HEADER

/*! \brief RX interrupt handler
 *
 */
void callback_uart_rx_interrupt();

void handleNewLine(void);

#endif /* UART_TO_ESP_INTERNAL_HEADER */
