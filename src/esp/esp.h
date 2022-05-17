#ifndef SENSOR_BOARD_ESP_H
#define SENSOR_BOARD_ESP_H

#include <stdbool.h>

// Speed at which the uart response buffer is checked in MS
#define REFRESH_RESPOND_IN_MS 500

bool ESP_SendCommand(char *cmd, char *expectedResponse, int timeoutMs);

bool ESP_SendCommandAndGetResponse(char *cmd, char *expectedResponse, int timeoutMs, char * response);

void ESP_SoftReset(void);

bool ESP_CheckIsResponding(void);

#endif //SENSOR_BOARD_ESP_H
