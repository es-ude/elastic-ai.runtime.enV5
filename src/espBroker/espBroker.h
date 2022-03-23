#ifndef SENSOR_BOARD_ESPBROKER_H
#define SENSOR_BOARD_ESPBROKER_H

#include "posting.h"
#include <stdbool.h>

#define MAX_TOPIC_NAME_LENGTH 20

void ESP_MQTT_SetClientId(char *clientId);

void ESP_MQTT_ConnectToBroker(char *host, char *port);

void ESP_MQTT_Disconnect(bool force);

void ESP_MQTT_Publish(Posting message);

void ESP_MQTT_Subscribe(char *topic);

void ESP_MQTT_Unsubscribe(char *topic);

bool ESP_MQTT_IsResponseAvailable();

void ESP_MQTT_GetResponse(Posting *response);

#endif //SENSOR_BOARD_ESPBROKER_H
