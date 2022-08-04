#ifndef UART_TO_ESP_H
#define UART_TO_ESP_H

#include <stdbool.h>

#define UART_BUFFER_SIZE 1024

typedef struct uartInstance uartToESP_Instance;

typedef enum { ParityNone, ParityEven, ParityOdd } uartToESP_parity;

typedef struct {
    char name[15];
    uartToESP_Instance *uartInstance;
    unsigned int uartId;
    unsigned int tx_pin;
    unsigned int rx_pin;
    unsigned int baudrate_set;
    unsigned int baudrate_actual;
    unsigned int data_bits;
    unsigned int stop_bits;
    uartToESP_parity parity;
    char receive_buffer[UART_BUFFER_SIZE];
    unsigned int receivedCharacter_count;
} UARTDevice;

void uartToEsp_Init(void);

void uartToESP_SendCommand(char *, char *string);

void uartToESP_Println(char *data);

void uartToESP_SetMQTTReceiverFunction(void (*receive)(char *));

bool uartToESP_IsBusy(void);

bool uartToESP_ResponseArrived(void);

void uartToESP_FreeCommand(void);

#endif // UART_TO_ESP_H
