#ifndef SENSOR_BOARD_ESP_H
#define SENSOR_BOARD_ESP_H

#include <stdbool.h>

// Speed at which the uart response buffer is checked in MS
#define REFRESH_RESPOND_IN_MS 250

#define ESP_CHIP_OK 1
#define ESP_CHIP_NOT_OK 0
#define CONNECTED 1
#define NOT_CONNECTED 0

typedef struct {
    // Can the two chips communicate?
    int ChipStatus;
    // Is the esp connected to a Wi-Fi
    int WIFIStatus;
    // is the board connected to an MQTT Broker
    int MQTTStatus;
} ESP_Status_t;

extern ESP_Status_t ESP_Status;

/*! \brief initializes the ESP connection
 *
 * Initializes UART connection, resets the ESP and checks if the connections is
 * working until there is a response. Then the command echo is  disabled and the
 * ESP is set to single connection mode. The ChipStatus is set to ESP_CHIP_OK.
 */
void ESP_Init(void);

/*! \brief send a command to the ESP
 *
 * Send a command via UART to the ESP and waits the given amount of time for an
 * expected answer.
 *
 * \param cmd command to be send
 * \param expectedResponse response that is expected to arrive if the command
 * succeeded \param timeoutMs number of milliseconds to wait for the expected
 * response \return true if the expected response arrived in time
 */
bool ESP_SendCommand(char *cmd, char *expectedResponse, int timeoutMs);

/*! \brief sends reset command to ESP */
void ESP_SoftReset(void);

/*! \brief check if ESP responds to commands */
bool ESP_CheckIsResponding(void);

/*! \brief only for the MQTT broker library
 *
 * Used by the MQTT broker to set its function to receive MQTT messages from
 * UART.
 */
void ESP_SetMQTTReceiverFunction(void (*receive)(char *));

#endif // SENSOR_BOARD_ESP_H
