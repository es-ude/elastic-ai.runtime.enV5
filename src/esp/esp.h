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

void ESP_Init(void);

bool ESP_SendCommand(char *cmd, char *expectedResponse, int timeoutMs);

bool ESP_SendCommandForce(char *cmd, char *expectedResponse, int timeoutMs);

void ESP_SoftReset(void);

bool ESP_CheckIsResponding(void);

void ESP_SetMQTTReceiverFunction(void (*receive)(char *));

#endif //SENSOR_BOARD_ESP_H
