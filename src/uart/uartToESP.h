#ifndef UART_TO_ESP_H
#define UART_TO_ESP_H

#include <stdbool.h>

#define UART_BUFFER_SIZE 1024

typedef struct uartInstance UartInstance;

typedef enum { UartParity_NONE, UartParity_EVEN, UartParity_ODD } UartParity;

typedef struct {
    char name[15];
    UartInstance *uartInstance;
    unsigned int uartId;
    unsigned int tx_pin;
    unsigned int rx_pin;
    unsigned int baudrate_set;
    unsigned int baudrate_actual;
    unsigned int data_bits;
    unsigned int stop_bits;
    UartParity parity;
    char receive_buf[UART_BUFFER_SIZE];
    unsigned int receive_count;
} UARTDevice;

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
    .parity = UartParity_NONE,
};

void uartToEsp_Init(void);

void uartToESP_SendCommand(char *, char *string);

void uartToESP_Println(char *data);

void uartToESP_SetMQTTReceiverFunction(void (*receive)(char *));

bool uartToESP_IsBusy(void);

bool uartToESP_ResponseArrived(void);

void uartToESP_FreeCommand(void);

#endif // UART_TO_ESP_H
