#ifndef ENV5_UART_HEADER
#define ENV5_UART_HEADER

#include <stdbool.h>
#include <stdint.h>

#define UART_BUFFER_SIZE 2048

typedef struct uart_inst uart_inst_t;
typedef unsigned int uint;

enum uartParity { NoneParity = 0, OddParity = 1, EvenParity = 2 };
typedef enum uartParity uartParity_t;

//! We highly recommend using the "enV5_hw_configuration_rev_[x]" -library
typedef struct uartConfiguration {
    uart_inst_t *uartInstance;
    uint8_t txPin;
    uint8_t rxPin;
    uint32_t baudrate;
    uint dataBits;
    uint stopBits;
    uartParity_t parity;
    char receiveBuffer[UART_BUFFER_SIZE];
    uint receivedCharacter_count;
} uartConfiguration_t;

typedef enum {
    UART_NO_ERROR = 0x00,
    UART_IS_BUSY = 0x01,
} uartErrorCode_t;

/*!
 * @brief initialize UART to communicate with ESP
 *
 * @IMPORTANT We highly recommend using the "enV5_hw_configuration_rev_[x]" -library
 *
 * @param uartConfig[in] struct that contains the UART configuration
 */
void uartInit(uartConfiguration_t *uartConfig);

/*!
 * @brief method to set function which handles UART receive interrupt for MQTT
 * @IMPORTANT We highly recommend using the "enV5_hw_configuration_rev_[x]" -library
 * @param receive: function for interrupt handle
 */
void uartSetMqttReceiverFunction(void (*receive)(char *));

/*!
 * @brief method to set function which handles UART receive interrupt for HTTP
 * @param receive: function for interrupt handle
 */
void uartSetHTTPReceiverFunction(void (*receive)(char *));

/*!
 * @brief send Command to ESP module via UART
 *
 * @IMPORTANT call `uartFreeCommandBuffer(void) to unblock UART after transmission
 *
 * @param uartConfig               Pointer to configuration of used UART
 * @param command[char *]          Pointer to char array which holds the ESP command to send
 * @param expectedResponse[char *] Pointer to char array which holds the successful response
 * @return                         0x00 if no error
 */
uartErrorCode_t uartSendCommand(const uartConfiguration_t *uartConfig, char *command,
                                char *expectedResponse);

/*!
 * @brief function to check if the correct response was received from the ESP module
 * @return [bool] true=correct response, false=else
 */
bool uartCorrectResponseArrived(void);

/*!
 * @brief free command buffer and unblock UART
 * @IMPORTANT Has to be called after every transaction, when finished
 */
void uartFreeCommandBuffer(void);

/*!
 * @brief FreeRTOS task to handle UART Rx
 * @IMPORTANT Has to be registered as a FreeRTOS TASK
 *
 * After the initialization, this task handles the UART receive logic outside of the interrupt.
 */
_Noreturn void uartReceiverTask(void);

#endif /* ENV5_UART_HEADER */
