#define SOURCE_FILE "UART-TO-ESP"

#include "uartToESP.h"
#include "uartToESP_internal.h"
#include "common.h"
#include "hardware/gpio.h"
#include "hardware/irq.h"
#include "hardware/uart.h"
#include <string.h>

/* region VARIABLES */
volatile bool lastReceivedCharacterWasReturn = false;
volatile bool correctResponseReceived = false;

char *commandToSend = "\0";
char *expectedResponseFromEsp = "\0";

void (*uartToESP_MQTT_Broker_Receive)(char *) = NULL;
/* endregion */

/* region HEADER FUNCTION IMPLEMENTATIONS */

void uartToEsp_Init(void) {
    if (device.uartId == 0)
        device.uartInstance = (uartToESP_Instance *)uart0;
    else if (device.uartId == 1)
        device.uartInstance = (uartToESP_Instance *)uart1;
    else
        return;

    // Set the TX and RX pins by using the function select on the GPIO
    // Set datasheet for more information on function select
    gpio_set_function(device.tx_pin, GPIO_FUNC_UART);
    gpio_set_function(device.rx_pin, GPIO_FUNC_UART);

    // Set up our UART with requested baud rate.
    // The call will return the actual baud rate selected, which will be as close as possible to
    // that requested
    device.baudrate_actual = uart_init((uart_inst_t *)device.uartInstance, device.baudrate_set);

    // Set UART flow control CTS/RTS, we don't want these, so turn them off
    uart_set_hw_flow((uart_inst_t *)device.uartInstance, false, false);

    // Set our data format
    uart_set_format((uart_inst_t *)device.uartInstance, device.data_bits, device.stop_bits,
                    (uart_parity_t)device.parity);

    // Turn off FIFO's - we want to do this character by character
    uart_set_fifo_enabled((uart_inst_t *)device.uartInstance, false);

    // And set up and enable the interrupt handlers
    irq_set_exclusive_handler(UART1_IRQ, callback_uart_rx_interrupt);
    irq_set_enabled(UART1_IRQ, true);

    // Now enable the UART to send interrupts - RX only
    uart_set_irq_enables((uart_inst_t *)device.uartInstance, true, false);

    device.receivedCharacter_count = 0;
    device.receive_buffer[0] = '\0';
}

void uartToESP_SetMQTTReceiverFunction(void (*receive)(char *)) {
    uartToESP_MQTT_Broker_Receive = receive;
}

void uartToESP_SendCommand(char *command, char *expectedResponse) {
    commandToSend = command;
    correctResponseReceived = false;
    expectedResponseFromEsp = expectedResponse;
    uartToESP_Println(commandToSend);
}

void uartToESP_Println(char *data) {
    uart_puts((uart_inst_t *)device.uartInstance, data);
    uart_puts((uart_inst_t *)device.uartInstance, "\r\n");
}

bool uartToESP_IsBusy(void) {
    if (strcmp(commandToSend, "\0") == 0)
        return false;
    return true;
}

bool uartToESP_ResponseArrived(void) {
    return correctResponseReceived;
}

void uartToESP_FreeCommand(void) {
    commandToSend = "\0";
}

/* endregion */

/* region INTERNAL HEADER FUNCTION IMPLEMENTATIONS */

void handleNewLine(void) {
    if (strlen(device.receive_buffer) != 0) {
        if (strncmp("+MQTTSUBRECV", device.receive_buffer, 12) == 0 &&
            uartToESP_MQTT_Broker_Receive != NULL) {
            uartToESP_MQTT_Broker_Receive(device.receive_buffer);
            correctResponseReceived = true;
        } else if (strncmp(expectedResponseFromEsp, device.receive_buffer,
                           strlen(expectedResponseFromEsp)) == 0) {
            correctResponseReceived = true;
        } else {
            PRINT_DEBUG("Received message was: %s", device.receive_buffer)
        }
    }
}

void callback_uart_rx_interrupt() {
    while (uart_is_readable((uart_inst_t *)device.uartInstance)) {
        char receivedCharacter = uart_getc((uart_inst_t *)device.uartInstance);

        if (receivedCharacter == '\n' || receivedCharacter == '\r' || receivedCharacter == '\0') {
            if (lastReceivedCharacterWasReturn && receivedCharacter == '\n') {
                // Line End reached
                handleNewLine();
                device.receivedCharacter_count = 0;
            } else if (receivedCharacter == '\r') {
                // store for next call
                lastReceivedCharacterWasReturn = true;
            }
        } else if (receivedCharacter == '>' && device.receivedCharacter_count == 1) {
            handleNewLine();
            device.receivedCharacter_count = 0;
        } else {
            // Store new character in buffer
            device.receive_buffer[device.receivedCharacter_count] = receivedCharacter;
            device.receivedCharacter_count++;

            // store for next call
            lastReceivedCharacterWasReturn = false;
        }

        // Neutralize buffer for next character
        device.receive_buffer[device.receivedCharacter_count] = '\0';
    }
}

/* endregion */
