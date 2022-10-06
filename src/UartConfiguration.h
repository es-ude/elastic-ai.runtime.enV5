#ifndef ENV5_UARTCONFIGURATION_H
#define ENV5_UARTCONFIGURATION_H

#include "Uart.h"

static UARTDevice uartToEspDevice = {
    // Below depends on the hardware connection
    // you should modify it according to your hardware
    .name = "uart_to_esp32",
    .uartId = 1,
    .tx_pin = 4,
    .rx_pin = 5,

    // Below depends on the firmware on the esp32 module
    .baudrate_set = 115200,
    .baudrate_actual = 0,
    .data_bits = 8,
    .stop_bits = 1,
    .parity = NoneParity,
};

#endif /* ENV5_UARTCONFIGURATION_H */
