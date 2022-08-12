#ifndef UART_TO_ESP_HEADER
#define UART_TO_ESP_HEADER

#include <stdbool.h>

#define UART_BUFFER_SIZE 1024

typedef struct uartInstance uartToESP_Instance;
typedef unsigned int uint;

typedef enum { NoneParity = 0, OddParity = 1, EvenParity = 2 } uartToESP_parity;

typedef struct {
    char name[15];
    uartToESP_Instance *uartInstance;
    uint uartId;
    uint tx_pin;
    uint rx_pin;
    uint baudrate_set;
    uint baudrate_actual;
    uint data_bits;
    uint stop_bits;
    uartToESP_parity parity;
    char receive_buffer[UART_BUFFER_SIZE];
    uint receivedCharacter_count;
} UARTDevice;

/*! \brief initialize UART to communicate with ESP
 *
 * @param device[UARTDevice] struct that contains the UART configuration
 */
void uartToEsp_Init(UARTDevice *device);

/*! \brief method to set function which handles UART receive interrupt
 *
 * @param receive function for interrupt handle
 */
void uartToESP_SetMQTTReceiverFunction(void (*receive)(char *));

/*! \brief send Command to ESP module via UART
 *
 * IMPORTANT: call `uartToESP_freeCommandBuffer(void) to unblock UART after transmission
 *
 * @param command[char *]          Pointer to char array which holds the ESP command to send
 * @param expectedResponse[char *] Pointer to char array which holds the successful response
 */
void uartToESP_sendCommand(char *command, char *expectedResponse);

/*! \brief function to check if the UART is currently used
 *
 * @return [bool] true=currently blocked, false=currently free
 */
bool uartToESP_isBusy(void);

/*! \brief function to check if the correct response was received from the ESP module
 *
 * @return [bool] true=correct response, false=else
 */
bool uartToESP_correctResponseArrived(void);

/*! \brief free command buffer and unblock UART
 *
 *  IMPORTANT: Has to be called after every transaction, when finished
 */
void uartToESP_freeCommandBuffer(void);

#endif /* UART_TO_ESP_HEADER */
