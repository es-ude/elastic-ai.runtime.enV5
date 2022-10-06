#ifndef ENV5_UART_CONFIGURATION_HEADER
#define ENV5_UART_CONFIGURATION_HEADER

#include "Uart.h"

static uartDevice_t uartDevice = {
    // Below depends on the hardware connection
    // you should modify it according to your hardware
    .name = "uart_to_esp32",
    .uartId = 1,
    .txPin = 4,
    .rxPin = 5,

    // Below depends on the firmware on the esp32 module
    .baudrateSet = 115200,
    .baudrateActual = 0,
    .dataBits = 8,
    .stopBits = 1,
    .parity = NoneParity,
};

#endif /* ENV5_UART_CONFIGURATION_HEADER */
