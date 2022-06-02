#ifndef UART_TO_ESP_H
#define UART_TO_ESP_H

#include <stdbool.h>

#define UART_BUFFER_SIZE 1024

typedef struct {
    char cmd[512];
    bool responseArrived;
    char expectedResponse[512];
    char data[512];
} esp_command;

typedef struct uartInstance UartInstance;

typedef enum {
    UartParity_NONE,
    UartParity_EVEN,
    UartParity_ODD
} UartParity;

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

void uartToEsp_Init(UARTDevice device);

void uartToESP_SendCommand(char *, char *string);

void uartToESP_Println(char *data);

void uartToESP_SetMQTTReceiverFunction(void (*receive)(char *));

bool uartToESP_IsBusy(void);

bool uartToESP_ResponseArrived(void);

void uartToESP_Free(void);

#endif // UART_TO_ESP_H
