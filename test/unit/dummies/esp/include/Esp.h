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
    volatile int WIFIStatus; /*!< Is the esp connected to a Wi-Fi */
    volatile int MQTTStatus; /*!< is the board connected to an MQTT Broker */
};
typedef struct espStatus espStatus_t;

extern volatile espStatus_t espStatus;

enum {
    ESP_NO_ERROR = 0x00,
    ESP_WRONG_ANSWER_RECEIVED = 0x01,
    ESP_UART_IS_BUSY = 0x02,
};
typedef uint8_t espErrorCode_t;

/*! \brief initializes the ESP connection
 *
 * Initializes UART connection, resets the ESP and checks if the connections is
 * working until there is a response. Then the command echo is  disabled and the
 * ESP is set to single connection mode. The ChipStatus is set to ESP_CHIP_OK.
 */
void espInit(void);

/*! \brief send a command to the ESP
 *
 * Send a command via UART to the ESP and waits the given amount of time for an
 * expected answer.
 *
 * \param cmd command to be send
 * \param expectedResponse response that is expected to arrive if the command succeeded
 * \param timeoutMs number of milliseconds to wait for the expected response
 * \return errorCode, 0 if no error occurred
 */
espErrorCode_t espSendCommand(char *cmd, char *expectedResponse, int timeoutMs);

/*! \brief only for the MQTT broker library
 *
 * Used by the MQTT broker to set its function to receive MQTT messages from
 * UART.
 */
void espSetMqttReceiverFunction(void (*receive)(char *));

void espSetHTTPReceiverFunction(void (*receive)(char *));

#endif // ENV5_ESP_HEADER
