#ifndef SENSOR_BOARD_MQTT_BROKER_H
#define SENSOR_BOARD_MQTT_BROKER_H

#include "posting.h"
#include <stdbool.h>
#include "subscriber.h"

#define MAX_SUBSCRIBER 100

typedef struct Subscription {
    char *topic;
    Subscriber subscriber;
} Subscription;

void MQTT_Broker_setBrokerDomain(char *ID);

void MQTT_Broker_SetClientId(char *clientId);

void MQTT_Broker_Receive(char * response);

void MQTT_Broker_ConnectToBroker(char *host, char *port);

void MQTT_Broker_Disconnect(bool force);

#endif //SENSOR_BOARD_MQTT_BROKER_H