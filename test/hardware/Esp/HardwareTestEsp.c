#include <stdio.h>

#include "hardware/gpio.h"
#include "hardware/uart.h"
#include "pico/stdlib.h"
#include "string.h"

uart_inst_t *uartInstance = uart1;
int baudrate = 115200;
uint8_t dataBits = 8;
uint8_t stopBits = 1;
uint8_t txPin = 4;
uint8_t rxPin = 5;

#define bufferSize 32
char buffer[bufferSize];
int receivedChars = 0;
bool espAnswered = false;

void uartInitInternal() {
    gpio_set_function(txPin, GPIO_FUNC_UART);
    gpio_set_function(rxPin, GPIO_FUNC_UART);

    uart_init(uartInstance, baudrate);
    uart_set_hw_flow(uartInstance, false, false);

    uart_set_format(uartInstance, dataBits, stopBits, UART_PARITY_NONE);
    uart_set_fifo_enabled(uartInstance, true);
}

bool checkForOkFromEsp() {
    while (uart_is_readable(uartInstance)) {
        char receivedCharacter = uart_getc(uartInstance);
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
    uartInitInternal();
    while (!espAnswered) {
        uart_puts(uartInstance, "AT\r\n");
        sleep_ms(1000);
        if (checkForOkFromEsp()) {
            espAnswered = true;
        }
        sleep_ms(1000);
        printf("Buffer: %s\n", buffer);
        sleep_ms(1000);
    }
    printf("ESP responded with OK!\n");
}
