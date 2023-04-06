#ifndef ENV5_UART_HEADER
#define ENV5_UART_HEADER

#include <stdbool.h>
#include <stdint.h>

#define UART_BUFFER_SIZE 2048

typedef struct uartInstance uartInstance_t;
typedef unsigned int uint;

enum uartParity { NoneParity = 0, OddParity = 1, EvenParity = 2 };
typedef enum uartParity uartParity_t;

struct uartDevice {
    char name[15];
    uartInstance_t *uartInstance;
    uint uartId;
    uint txPin;
    uint rxPin;
    uint baudrateSet;
    uint baudrateActual;
    uint dataBits;
    uint stopBits;
    uartParity_t parity;
    char receiveBuffer[UART_BUFFER_SIZE];
    uint receivedCharacter_count;
};
typedef struct uartDevice uartDevice_t;

enum {
    UART_NO_ERROR = 0x00,
    UART_IS_BUSY = 0x01,
};
typedef uint8_t uartErrorCode_t;

/*! \brief initialize UART to communicate with ESP
 *
 * @param device[UARTDevice] struct that contains the UART configuration
 */
void uartInit(uartDevice_t *device);

/*! \brief method to set function which handles UART receive interrupt for MQTT
 *
 * @param receive function for interrupt handle
 */
void uartSetMqttReceiverFunction(void (*receive)(char *));

/*! \brief method to set function which handles UART receive interrupt for HTTP
 *
 * @param receive function for interrupt handle
 */
void uartSetHTTPReceiverFunction(void (*receive)(char *));

/*! \brief send Command to ESP module via UART
 *
 * IMPORTANT: call `uartFreeCommandBuffer(void) to unblock UART after transmission
 *
 * @param command[char *]          Pointer to char array which holds the ESP command to send
 * @param expectedResponse[char *] Pointer to char array which holds the successful response
 * @return                         0x00 if no error
 */
uartErrorCode_t uartSendCommand(char *command, char *expectedResponse);

/*! \brief function to check if the correct response was received from the ESP module
 *
 * @return [bool] true=correct response, false=else
 */
bool uartCorrectResponseArrived(void);

/*! \brief free command buffer and unblock UART
 *
 *  IMPORTANT: Has to be called after every transaction, when finished
 */
void uartFreeCommandBuffer(void);

#endif /* ENV5_UART_HEADER */
