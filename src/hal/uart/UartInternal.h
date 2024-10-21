#ifndef ENV5_UART_INTERNAL_HEADER
#define ENV5_UART_INTERNAL_HEADER

/*! \brief RX interrupt handler
 *
 */
//_Noreturn void uartInternalCallbackUartRxInterrupt();

void uartInternalHandleNewLine(uartConfiguration_t *uartConfig);

void checkAndHandleNewChar(uartConfiguration_t *uartConfig);

void setNewUARTInterrupt(void);

#endif /* ENV5_UART_INTERNAL_HEADER */
