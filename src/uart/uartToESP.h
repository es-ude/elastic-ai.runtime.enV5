#ifndef UART_TO_ESP_H
#define UART_TO_ESP_H

#include "stdbool.h"
#include "pico/types.h"

#define UART_BUFFER_SIZE 2048

typedef struct uartInstance UartInstance;
typedef enum {
    UartParity_NONE,
    UartParity_EVEN,
    UartParity_ODD
} UartParity;

typedef struct {
    char name[15];
    UartInstance *uartInstance;
    int uartId;
    int tx_pin;
    int rx_pin;
    int baudrate_set;
    uint baudrate_actual;
    int data_bits;
    int stop_bits;
    UartParity parity;
    char receive_buf[UART_BUFFER_SIZE];
    uint receive_count;
} UARTDevice;

void uartToEsp_Init(UARTDevice device);

void uartToESP_Println(char *data);

void uartToESP_ResetReceiveBuffer(void);

void uartToESP_CleanReceiveBuffer(void);

bool uartToESP_ResponseArrived(char *expectedResponse);

bool uartToESP_ReadLine(char *startAtString, char *data);

bool uartToESP_Read(char *startAtString, char *responseBuf, int length);

bool uartToESP_Cut(char *startAtString, char *responseBuf, int length);

// DO NOT USE except for debugging purposes
char *uartToESP_GetReceiveBuffer();

#endif // UART_TO_ESP_H
