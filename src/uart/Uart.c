#define SOURCE_FILE "UART-TO-ESP"

#include "Uart.h"
#include "Common.h"
#include "Gpio.h"
#include "UartInternal.h"
#include "hardware/irq.h"
#include "hardware/uart.h"
#include <string.h>
#include "FreeRtosTaskWrapper.h"

/* region VARIABLES */

static uartDevice_t *uartDevice;

volatile static bool uartLastReceivedCharacterWasReturn = false;
volatile static bool uartCorrectResponseReceived = false;

static char *uartCommandToSend = "\0";
static char *uartExpectedResponseFromEsp = "\0";

void (*uartMqttBrokerReceive)(char *) = NULL;
void (*uartHTPPReceive)(char *) = NULL;

/* endregion */

/* region HEADER FUNCTION IMPLEMENTATIONS */

void uartInit(uartDevice_t *device) {
    uartDevice = device;

    if (uartDevice->uartId == 0) {
        uartDevice->uartInstance = (uartInstance_t *)uart0;
    } else {
        uartDevice->uartInstance = (uartInstance_t *)uart1;
    }

    // Set the TX and RX pins to UART by using the function select on the GPIO
    gpioSetPinFunction(uartDevice->txPin, GPIO_FUNCTION_UART);
    gpioSetPinFunction(uartDevice->rxPin, GPIO_FUNCTION_UART);

    // Set up our UART with requested baud rate.
    // The call will return the actual baud rate selected, which will be as close as possible
    // to that requested
    uartDevice->baudrateActual =
        uart_init((uart_inst_t *)uartDevice->uartInstance, uartDevice->baudrateSet);

    // Set UART flow control CTS/RTS, we don't want these, so turn them off
    uart_set_hw_flow((uart_inst_t *)uartDevice->uartInstance, false, false);

    // Set our data format
    uart_set_format((uart_inst_t *)uartDevice->uartInstance, uartDevice->dataBits,
                    uartDevice->stopBits, (uart_parity_t)uartDevice->parity);

    // Turn off FIFO's - we want to do this character by character
    uart_set_fifo_enabled((uart_inst_t *)uartDevice->uartInstance, false);

    // And set up and enable the interrupt handlers
    irq_set_exclusive_handler(UART1_IRQ, uartInternalCallbackUartRxInterrupt);
    irq_set_enabled(UART1_IRQ, true);

    // Now enable the UART to send interrupts - RX only
    uart_set_irq_enables((uart_inst_t *)uartDevice->uartInstance, true, false);

    // Clear internal buffer space
    uartDevice->receivedCharacter_count = 0;
    uartDevice->receiveBuffer[0] = '\0';
}

void uartSetMqttReceiverFunction(void (*receive)(char *)) {
    uartMqttBrokerReceive = receive;
}

void uartSetHTTPReceiverFunction(void (*receive)(char *)) {
    uartHTPPReceive = receive;
}

uartErrorCode_t uartSendCommand(char *command, char *expectedResponse) {
    // check if UART is currently occupied
    if (strcmp(uartCommandToSend, "\0") != 0) {
        PRINT("UART is busy. Can't send command %s.", command)
        return UART_IS_BUSY;
    }

    // reset internal buffer
    uartCommandToSend = command;
    uartCorrectResponseReceived = false;
    uartExpectedResponseFromEsp = expectedResponse;

    // send command over uart
    PRINT_DEBUG("COMMAND: %s", command)
    uart_puts((uart_inst_t *)uartDevice->uartInstance, command);
    uart_puts((uart_inst_t *)uartDevice->uartInstance, "\r\n");

    return UART_NO_ERROR;
}

bool uartCorrectResponseArrived(void) {
    return uartCorrectResponseReceived;
}

void uartFreeCommandBuffer(void) {
    uartCommandToSend = "\0";
}

/* endregion */

/* region INTERNAL HEADER FUNCTION IMPLEMENTATIONS */

void uartInternalHandleNewLine(void) {
    if (strlen(uartDevice->receiveBuffer) != 0) {
        if (strncmp("+MQTTSUBRECV", uartDevice->receiveBuffer, 12) == 0 &&
            uartMqttBrokerReceive != NULL) {
            // handle Received MQTT message -> pass to correct subscriber
            uartMqttBrokerReceive(uartDevice->receiveBuffer);
        }
        if ((strncmp("++HTTPCGET", uartDevice->receiveBuffer, 10) == 0 ||
             strncmp("+HTTPCGET", uartDevice->receiveBuffer, 9) == 0) &&
            uartHTPPReceive != NULL) {
            // handle HTTP message
            uartHTPPReceive(uartDevice->receiveBuffer);
        }
        if (strncmp(uartExpectedResponseFromEsp, uartDevice->receiveBuffer,
                    strlen(uartExpectedResponseFromEsp)) == 0) {
            PRINT_DEBUG("Expected message received: %s", uartDevice->receiveBuffer)
            uartCorrectResponseReceived = true;
        } else {
            if(strlen(uartDevice->receiveBuffer) < 15) {
                PRINT_DEBUG("Received message was: %s", uartDevice->receiveBuffer)
            }
        }
    }
}

void uartInternalCallbackUartRxInterrupt() {
    while (uart_is_readable((uart_inst_t *)uartDevice->uartInstance)) {
        char receivedCharacter = uart_getc((uart_inst_t *)uartDevice->uartInstance);

        if (receivedCharacter == '\n' || receivedCharacter == '\r' || receivedCharacter == '\0') {
            if (uartLastReceivedCharacterWasReturn && receivedCharacter == '\n') {
                // Line End reached
                uartInternalHandleNewLine();
                uartDevice->receivedCharacter_count = 0;
            } else if (receivedCharacter == '\r') {
                // store for next call
                uartLastReceivedCharacterWasReturn = true;
            }
        } else if (receivedCharacter == '>' && uartDevice->receivedCharacter_count == 1) {
            uartInternalHandleNewLine();
            uartDevice->receivedCharacter_count = 0;
        } else {
            // Store new character in buffer
            uartDevice->receiveBuffer[uartDevice->receivedCharacter_count] = receivedCharacter;
            uartDevice->receivedCharacter_count++;

            // store for next call
            uartLastReceivedCharacterWasReturn = false;
        }

        // Neutralize buffer for next character
        uartDevice->receiveBuffer[uartDevice->receivedCharacter_count] = '\0';
    }
}

/* endregion */
