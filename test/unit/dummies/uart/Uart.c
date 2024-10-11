#define SOURCE_FILE "UART-TO-ESP"

#include "Uart.h"
#include "Common.h"

void uartInit(uartConfiguration_t *uartConfig) {
    // Just here to satisfy the compiler
}

void uartSetMqttReceiverFunction(void (*receive)(char *)) {
    // Just here to satisfy the compiler
}

void uartSetHTTPReceiverFunction(void (*receive)(char *)) {
    // Just here to satisfy the compiler
}

uartErrorCode_t uartSendCommand(const uartConfiguration_t *uartConfig, char *command,
                                char *expectedResponse) {
    PRINT("UART send Message: >%s<", command);
    return UART_NO_ERROR;
}

bool uartCorrectResponseArrived(void) {
    return true;
}

void uartFreeCommandBuffer(void) {
    // Just here to satisfy the compiler
}

_Noreturn void uartReceiverTask(void) {
    // Just here to satisfy the compiler
    while (true) {}
}
