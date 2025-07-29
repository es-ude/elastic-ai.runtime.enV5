#define SOURCE_FILE "UART-TO-ESP"
#include <stdlib.h>
#include <string.h>

#include "hardware/irq.h"
#include "hardware/uart.h"

#include "UartInternal.h"
#include "eai/Common.h"
#include "eai/hal/EnV5HwConfiguration.h"
#include "eai/hal/Gpio.h"
#include "eai/hal/Uart.h"

/* region VARIABLES */

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

static uartConfiguration_t uartConfig;

/* endregion  VARIABLES*/

/* region HEADER FUNCTION IMPLEMENTATIONS */

void uartInit(uartConfiguration_t configuration) {
    uartConfig = configuration;

    // Set the TX and RX pins to UART by using the function select on the GPIO
    gpioSetPinFunction(uartConfig.txPin, GPIO_FUNCTION_UART);
    gpioSetPinFunction(uartConfig.rxPin, GPIO_FUNCTION_UART);

    // Set up our UART with requested baud rate.
    // The call will return the actual baud rate selected,
    // which will be as close as possible to that requested
    uartConfig.uartInstance = UART_MODULE;
    uart_init(uartConfig.uartInstance, uartConfig.baudrate);

    // Set UART flow control CTS/RTS, we don't want these, so turn them off
    uart_set_hw_flow(uartConfig.uartInstance, false, false);

    // Set our data format
    uart_set_format(uartConfig.uartInstance, uartConfig.dataBits, uartConfig.stopBits,
                    uartConfig.parity);

    // Turn off FIFO's - we want to do this character by character
    uart_set_fifo_enabled(uartConfig.uartInstance, false);

    uartInternalCallbackUartRxInterrupt = checkAndHandleNewChar;
    // And set up and enable the interrupt handlers
    irq_set_exclusive_handler(UART1_IRQ, uartInternalCallbackUartRxInterrupt);
    irq_set_enabled(UART1_IRQ, true);

    // Now enable the UART to send interrupts - RX only
    uart_set_irq_enables(uartConfig.uartInstance, true, false);

    // Clear internal buffer space
    uartConfig.receivedCharacter_count = 0;
    uartConfig.receiveBuffer[0] = '\0';
}

void uartSetMqttReceiverFunction(void (*receive)(char *)) {
    uartMqttBrokerReceive = receive;
}

void uartSetHTTPReceiverFunction(void (*receive)(char *)) {
    uartHTTPReceive = receive;
}

uartErrorCode_t uartSendCommand(const uartConfiguration_t *uartConfig, char *command,
                                char *expectedResponse) {
    // check if UART is currently occupied
    if (strcmp(uartCommandToSend, "\0") != 0) {
        PRINT("UART is busy. Can't send command %s.", command);
        return UART_IS_BUSY;
    }

    // reset internal buffer
    PRINT_DEBUG("Reset Buffer");
    uartCommandToSend = command;
    uartCorrectResponseReceived = false;
    uartExpectedResponseFromEsp = expectedResponse;

    // required for HTTPCGET response handling
    if (strncmp("AT+HTTPCGET", command, 11) == 0) {
        PRINT_DEBUG("Add HTTPGET");
        uartAwaitingHttpGet++;
    }

    // send command over uart
    PRINT_DEBUG("Sending command: \"%s\"", command);
    uart_puts(uartConfig->uartInstance, command);
    // send \r\n because AT command always ends with CR-LF
    uart_puts(uartConfig->uartInstance, "\r\n");
    PRINT_DEBUG("command send");

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

void uartInternalHandleNewLine() {
    /* IMPORTANT:
     *   Don't use print statements for debugging!!
     *   Print statements will cause timing/buffer issues
     */

    if (0 == strlen(uartConfig.receiveBuffer)) {
        PRINT_DEBUG("Empty Buffer");
        return;
    }
    if (strncmp("+MQTTSUBRECV", uartConfig.receiveBuffer, 12) == 0) {
        // handle Received MQTT message -> pass to correct subscriber
        if (uartMqttBrokerReceive != NULL) {
            uartMqttBrokerReceive(uartConfig.receiveBuffer);
        }
    } else if (strncmp("+HTTPCGET", uartConfig.receiveBuffer, 9) == 0) {
        uartCurrentMessageIsHttpResponse = false;
        uartAwaitingHttpGet--;
        // handle HTTP message
        if (uartHTTPReceive != NULL) {
            uartHTTPReceive(uartConfig.receiveBuffer);
        }
    } else if (strncmp("+CWJAP", uartConfig.receiveBuffer, 6) == 0) {
        PRINT_DEBUG("Message: %s", uartConfig.receiveBuffer);
        uartCorrectResponseReceived = true;
    }
    if (strncmp(uartExpectedResponseFromEsp, uartConfig.receiveBuffer,
                strlen(uartExpectedResponseFromEsp)) == 0) {
        uartCorrectResponseReceived = true;
    }
}

void checkAndHandleNewChar() {
    /* IMPORTANT:
     *   Don't use print statements for debugging!!
     *   Print statements will cause timing/buffer issues
     */

    while (uart_is_readable((uart_inst_t *)uartConfig.uartInstance)) {
        char receivedCharacter = uart_getc((uart_inst_t *)uartConfig.uartInstance);

        // handle HTTP-GET
        /* IMPORTANT:
         *   has to be done before others,
         *   because binary data payload can contain CR/LF sequenc without!
         */
        if (uartCurrentMessageIsHttpResponse && uartHttpResponseLength > 0) {
            // append new character to buffer
            uartConfig.receiveBuffer[uartConfig.receivedCharacter_count] = receivedCharacter;

            // increase buffer last character pointer for next character
            uartConfig.receivedCharacter_count++;
            uartConfig.receiveBuffer[uartConfig.receivedCharacter_count] = '\0';

            uartHttpResponseLength--;
            return;
        }
        if (!uartCurrentMessageIsHttpResponse && uartAwaitingHttpGet > 0 &&
            receivedCharacter == ',') {
            if (strncmp("+HTTPCGET", uartConfig.receiveBuffer, 9) == 0) {
                uartCurrentMessageIsHttpResponse = true;
                char *startSize = strstr(uartConfig.receiveBuffer, ":") + 1;
                char *endSize = strstr(uartConfig.receiveBuffer, ",");
                uartHttpResponseLength = strtol(startSize, &endSize, 10);
            }
        }

        // check for Response end: CR-LF
        // CR = \r = 0b00001101 = 0x0D -> Carriage Return
        // LF = \n = 0b00001010 = 0x0A -> New Line
        if (uartLastReceivedCharacterWasReturn && receivedCharacter == '\n') {
            // remove last \r from response buffer
            uartConfig.receiveBuffer[uartConfig.receivedCharacter_count--] = '\0';
            // call response handle
            uartInternalHandleNewLine();
            // reset response buffer
            uartConfig.receivedCharacter_count = 0;
            uartConfig.receiveBuffer[uartConfig.receivedCharacter_count] = '\0';
            return;
        } else if (receivedCharacter == '\r') {
            uartLastReceivedCharacterWasReturn = true;
        } else {
            uartLastReceivedCharacterWasReturn = false;
        }

        // append new character to buffer
        uartConfig.receiveBuffer[uartConfig.receivedCharacter_count] = receivedCharacter;

        // increase buffer last character pointer for next character
        uartConfig.receivedCharacter_count++;
        uartConfig.receiveBuffer[uartConfig.receivedCharacter_count] = '\0';
    }
}

/* endregion INTERNAL HEADER FUNCTION IMPLEMENTATIONS */
