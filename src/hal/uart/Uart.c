#define SOURCE_FILE "UART-TO-ESP"

#include "Uart.h"
#include "Common.h"
#include "Gpio.h"
#include "UartInternal.h"

#include "hardware/irq.h"
#include "hardware/uart.h"

#include <stdlib.h>
#include <string.h>

/* region VARIABLES */

static uartDevice_t *uartDevice;

volatile static bool uartLastReceivedCharacterWasReturn = false;
volatile static bool uartCorrectResponseReceived = false;
volatile static uint16_t uartAwaitingHttpGet = 0;
volatile static bool uartCurrentMessageIsHttpResponse = false;
volatile static uint32_t uartHttpResponseLength = 0;

static char *uartCommandToSend = "\0";
static char *uartExpectedResponseFromEsp = "\0";

void (*uartMqttBrokerReceive)(char *) = NULL;
void (*uartHTTPReceive)(char *) = NULL;

void (*uartInternalCallbackUartRxInterrupt)(void);
/* endregion  VARIABLES*/

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
    // The call will return the actual baud rate selected, which will be as close as possible to
    // that requested
    uartDevice->baudrateActual =
        uart_init((uart_inst_t *)uartDevice->uartInstance, uartDevice->baudrateSet);

    // Set UART flow control CTS/RTS, we don't want these, so turn them off
    uart_set_hw_flow((uart_inst_t *)uartDevice->uartInstance, false, false);

    // Set our data format
    uart_set_format((uart_inst_t *)uartDevice->uartInstance, uartDevice->dataBits,
                    uartDevice->stopBits, (uart_parity_t)uartDevice->parity);

    // Turn off FIFO's - we want to do this character by character
    uart_set_fifo_enabled((uart_inst_t *)uartDevice->uartInstance, false);

    uartInternalCallbackUartRxInterrupt = checkAndHandleNewChar;
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
    uartHTTPReceive = receive;
}

uartErrorCode_t uartSendCommand(char *command, char *expectedResponse) {
    // check if UART is currently occupied
    if (strcmp(uartCommandToSend, "\0") != 0) {
        PRINT("UART is busy. Can't send command %s.", command)
        return UART_IS_BUSY;
    }

    // reset internal buffer
    PRINT_DEBUG("Reset Buffer")
    uartCommandToSend = command;
    uartCorrectResponseReceived = false;
    uartExpectedResponseFromEsp = expectedResponse;

    // required for HTTPCGET response handling
    if (strncmp("AT+HTTPCGET", command, 11) == 0) {
        PRINT_DEBUG("Add HTTPGET")
        uartAwaitingHttpGet++;
    }

    // send command over uart
    PRINT_DEBUG("Sending command: \"%s\"", command)
    uart_puts((uart_inst_t *)uartDevice->uartInstance, command);
    // send \r\n because AT command always ends with CR-LF
    uart_puts((uart_inst_t *)uartDevice->uartInstance, "\r\n");
    PRINT_DEBUG("command send")

    return UART_NO_ERROR;
}

bool uartCorrectResponseArrived(void) {
    return uartCorrectResponseReceived;
}

void uartFreeCommandBuffer(void) {
    uartCommandToSend = "\0";
}

/* endregion HEADER FUNCTION IMPLEMENTATIONS */

/* region INTERNAL HEADER FUNCTION IMPLEMENTATIONS */

/*! IMPORTANT: Don't use print statements for debugging!!
 *             Print statements will cause timing/buffer issues
 */
void uartInternalHandleNewLine(void) {
    if (0 == strlen(uartDevice->receiveBuffer)) {
        PRINT_DEBUG("Empty Buffer")
        return;
    }
    if (strncmp("+MQTTSUBRECV", uartDevice->receiveBuffer, 12) == 0) {
        // handle Received MQTT message -> pass to correct subscriber
        if (uartMqttBrokerReceive != NULL) {
            uartMqttBrokerReceive(uartDevice->receiveBuffer);
        }
    } else if (strncmp("+HTTPCGET", uartDevice->receiveBuffer, 9) == 0) {
        uartCurrentMessageIsHttpResponse = false;
        uartAwaitingHttpGet--;
        // handle HTTP message
        if (uartHTTPReceive != NULL) {
            uartHTTPReceive(uartDevice->receiveBuffer);
        }
    } else if (strncmp("+CWJAP", uartDevice->receiveBuffer, 6) == 0) {
        PRINT_DEBUG("Message: %s", uartDevice->receiveBuffer)
        uartCorrectResponseReceived = true;
    }
    if (strncmp(uartExpectedResponseFromEsp, uartDevice->receiveBuffer,
                strlen(uartExpectedResponseFromEsp)) == 0) {
        uartCorrectResponseReceived = true;
    }
}

/*! IMPORTANT: Don't use print statements for debugging!!
 *             Print statements will cause timing/buffer issues
 */
void checkAndHandleNewChar(void) {
    while (uart_is_readable((uart_inst_t *)uartDevice->uartInstance)) {
        char receivedCharacter = uart_getc((uart_inst_t *)uartDevice->uartInstance);

        // handle HTTP get
        if (uartCurrentMessageIsHttpResponse && uartHttpResponseLength > 0) {
            // append new character to buffer
            uartDevice->receiveBuffer[uartDevice->receivedCharacter_count] = receivedCharacter;

            // increase buffer last character pointer for next character
            uartDevice->receivedCharacter_count++;
            uartDevice->receiveBuffer[uartDevice->receivedCharacter_count] = '\0';

            uartHttpResponseLength--;
            return;
        }
        if (!uartCurrentMessageIsHttpResponse && uartAwaitingHttpGet > 0 &&
            receivedCharacter == ',') {
            if (strncmp("+HTTPCGET", uartDevice->receiveBuffer, 9) == 0) {
                uartCurrentMessageIsHttpResponse = true;
                char *startSize = strstr(uartDevice->receiveBuffer, ":") + 1;
                char *endSize = strstr(uartDevice->receiveBuffer, ",");
                uartHttpResponseLength = strtol(startSize, &endSize, 10);
            }
        }

        // check for Response end: CR-LF
        // CR = \r = 0b00001101 = 0x0D -> Carriage Return
        // LF = \n = 0b00001010 = 0x0A -> New Line
        if (uartLastReceivedCharacterWasReturn && receivedCharacter == '\n') {
            // remove last \r from response buffer
            uartDevice->receiveBuffer[uartDevice->receivedCharacter_count--] = '\0';
            // call response handle
            uartInternalHandleNewLine();
            // reset response buffer
            uartDevice->receivedCharacter_count = 0;
            uartDevice->receiveBuffer[uartDevice->receivedCharacter_count] = '\0';
            return;
        } else if (receivedCharacter == '\r') {
            uartLastReceivedCharacterWasReturn = true;
        } else {
            uartLastReceivedCharacterWasReturn = false;
        }

        // append new character to buffer
        uartDevice->receiveBuffer[uartDevice->receivedCharacter_count] = receivedCharacter;

        // increase buffer last character pointer for next character
        uartDevice->receivedCharacter_count++;
        uartDevice->receiveBuffer[uartDevice->receivedCharacter_count] = '\0';
    }
}

/* endregion INTERNAL HEADER FUNCTION IMPLEMENTATIONS */
