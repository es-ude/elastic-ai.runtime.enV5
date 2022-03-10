#ifndef SENSOR_BOARD_ESPTCP_H
#define SENSOR_BOARD_ESPTCP_H

#include <stdbool.h>

bool ESP_TCP_Open(char *target, char *port);
bool ESP_TCP_Close(bool force);
bool ESP_TCP_SendData(char *data, int timeoutMs);
bool ESP_TCP_IsResponseAvailable();
/*!
 * the return value is dynamically allocated!
 * use free() after your are finished processing the response
 */
char *ESP_TCP_GetResponse();

static char *ESP_TCP_CutResponseToBuffer();

#endif // SENSOR_BOARD_ESPTCP_H
