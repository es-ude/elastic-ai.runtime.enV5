#ifndef SENSOR_BOARD_ESP_H
#define SENSOR_BOARD_ESP_H

#include <stdbool.h>

// Speed at which the uart response buffer is checked in MS
#define REFRESH_RESPOND_IN_MS 250

void ESP_Init(void);

bool ESP_SendCommand(char *cmd, char *expectedResponse, int timeoutMs);

bool ESP_SendCommandForce(char *cmd, char *expectedResponse, int timeoutMs);

void ESP_SoftReset(void);

bool ESP_CheckIsResponding(void);

void ESP_SetMQTTReceiverFunction(void (*receive)(char *));

#endif //SENSOR_BOARD_ESP_H
