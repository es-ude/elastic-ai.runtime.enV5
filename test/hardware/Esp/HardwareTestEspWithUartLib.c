#include <stdio.h>

#include "hardware/uart.h"
#include "pico/stdlib.h"
#include "string.h"

#include "eai/hal/EnV5HwConfiguration.h"
#include "eai/hal/Uart.h"

static uartConfiguration_t uartConfig;

#define bufferSize 32
char buffer[bufferSize];
int receivedChars = 0;
bool espAnswered = false;

void uartInitInternal() {
    uartConfig.uartInstance = UART_MODULE;
    uartConfig.baudrate = UART_BAUDRATE;
    uartConfig.dataBits = UART_DATA_BITS;
    uartConfig.stopBits = UART_STOP_BITS;
    uartConfig.txPin = UART_TX_PIN;
    uartConfig.rxPin = UART_RX_PIN;
    uartConfig.parity = UART_PARITY_NONE;
}

bool checkForOkFromEsp() {
    while (uart_is_readable(uartConfig.uartInstance)) {
        char receivedCharacter = uart_getc(uartConfig.uartInstance);
        buffer[receivedChars] = receivedCharacter;

        if (receivedChars < bufferSize - 1) {
            buffer[receivedChars++] = receivedCharacter;
        }

        if (receivedCharacter == '\n') {
            buffer[receivedChars] = '\0';
            if (strstr(buffer, "OK") != NULL) {
                return true;
            }
            memset(buffer, 0, sizeof(buffer));
            receivedChars = 0;
        }
    }
    return false;
}

int main() {
    stdio_init_all();
    while (!stdio_usb_connected()) {}
    sleep_ms(1000);
    // use EnV5 Config for UART params
    uartInitInternal();

    // use our UART lib to initialize UART
    uartInit(uartConfig);
    printf("Uart initialized.\n");

    /*while (!espAnswered) {
        uart_puts(uartConfig.uartInstance, "AT\r\n");
        sleep_ms(1000);
        if (checkForOkFromEsp()) {
            espAnswered = true;
        }
        sleep_ms(1000);
        printf("Buffer: %s\n", buffer);
        sleep_ms(1000);
    }*/

    while (!uartCorrectResponseArrived()) {
        uart_puts(uartConfig.uartInstance, "AT\r\n");
        sleep_ms(1000);
        printf("Buffer: %s\n", buffer);
        printf("ESP answer not correct...\n");
        printf("Trying again.\n");
        sleep_ms(1000);
    }

    printf("ESP responded with OK!\n");
}
