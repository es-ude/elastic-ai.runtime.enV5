#ifndef SENSOR_BOARD_MQTT_BROKER_H
#define SENSOR_BOARD_MQTT_BROKER_H

#include <stdbool.h>

#define MAX_SUBSCRIBER 100

void MQTT_Broker_ConnectToBrokerUntilConnected(char *target, char *port, char *brokerDomain, char *clientID);

bool MQTT_Broker_ConnectToBroker(char *target, char *port, char *brokerDomain, char *clientID);

void MQTT_Broker_Disconnect(bool force);

void MQTT_Broker_setBrokerDomain(char *ID);

void MQTT_Broker_SetClientId(char *clientId);

void MQTT_Broker_Receive(char *response);

#endif //SENSOR_BOARD_MQTT_BROKER_H
