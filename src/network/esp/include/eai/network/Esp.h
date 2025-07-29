#ifndef ENV5_ESP_HEADER
#define ENV5_ESP_HEADER

#include <stdbool.h>
#include <stdint.h>

#define REFRESH_RESPOND_IN_MS 250 /*!< Speed at which the uart response buffer is checked in MS */

#define ESP_CHIP_OK 1
#define ESP_CHIP_NOT_OK 0
#define CONNECTED 1
#define NOT_CONNECTED 0

struct espStatus {
    volatile int ChipStatus; /*!< Can the two chips communicate? */
    volatile int WIFIStatus; /*!< Is the esp connected to a wifi network? */
    volatile int MQTTStatus; /*!< Is the board connected to a MQTT-Broker? */
};
typedef struct espStatus espStatus_t;
extern volatile espStatus_t espStatus;

typedef enum espErrorCode {
    ESP_NO_ERROR = 0x00,
    ESP_WRONG_ANSWER_RECEIVED = 0x01,
    ESP_UART_IS_BUSY = 0x02,
} espErrorCode_t;

/*!
 * @brief initializes the ESP
 */
void espInit(void);

/*!
 * @brief send a command to the ESP
 *
 * Send a command via UART to the ESP and waits the given amount of time for the expected answer.
 *
 * \param cmd command to be sent
 * \param expectedResponse response that is expected to arrive if the command succeeded
 * \param timeoutMs number of milliseconds to wait for the expected response
 * \return errorCode, 0 if no error occurred
 */
espErrorCode_t espSendCommand(char *cmd, char *expectedResponse, int timeoutMs);

/*!
 * @brief provides the MQTT UART handle
 *
 * @IMPORTANT
 *    Only for the uart library.
 *    Used by the MQTT library to set its function to receive MQTT messages from UART.
 */
void espSetMqttReceiverFunction(void (*receive)(char *));

/*!
 * @brief provides the HTTP UART handle
 *
 * @IMPORTANT
 *    Only for the uart library.
 *    Used by the HTTP library to set its function to receive HTTP responses from UART.
 */
void espSetHTTPReceiverFunction(void (*receive)(char *));

#endif // ENV5_ESP_HEADER
