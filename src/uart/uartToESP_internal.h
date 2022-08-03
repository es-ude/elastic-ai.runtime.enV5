#ifndef UART_TO_ESP_INTERNAL_HEADER
#define UART_TO_ESP_INTERNAL_HEADER

static UARTDevice device = {
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
    .parity = ParityNone,
};

/*! \brief RX interrupt handler
 *
 */
void callback_uart_rx_interrupt();

void handleNewLine(void);

#endif /* UART_TO_ESP_INTERNAL_HEADER */
