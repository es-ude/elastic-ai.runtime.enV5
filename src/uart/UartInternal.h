#ifndef INCLUDE_ENV5_UART_INTERNAL_HEADER
#warning Internal header file "UartInternal.h" included from elsewhere
#undef INCLUDE_ENV5_UART_INTERNAL_HEADER
#endif

#ifndef ENV5_UART_INTERNAL_HEADER
#define ENV5_UART_INTERNAL_HEADER

/*! \brief RX interrupt handler
 *
 */
void uartInternalCallbackUartRxInterrupt();

void uartInternalHandleNewLine(void);

#endif /* ENV5_UART_INTERNAL_HEADER */
