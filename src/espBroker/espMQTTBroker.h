#ifndef SENSOR_BOARD_ESPBROKER_H
#define SENSOR_BOARD_ESPBROKER_H

#include "posting.h"
#include <stdbool.h>
#include "subscriber.h"

#define MAX_TOPIC_NAME_LENGTH 20
#define MAX_SUBSCRIBER 100

typedef struct Subscription {
    char *topic;
    Subscriber subscriber;
} Subscription;

void ESP_MQTT_BROKER_SetClientId(char *clientId);

void ESP_MQTT_BROKER_ConnectToBroker(char *host, char *port);

void ESP_MQTT_BROKER_Disconnect(bool force);

#endif //SENSOR_BOARD_ESPBROKER_H
