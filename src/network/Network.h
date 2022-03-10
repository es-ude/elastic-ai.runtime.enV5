//
// Created by Lukas on 13.12.2021.
//

#ifndef NETWORK_H
#define NETWORK_H

#include <stdbool.h>
#include <stdint.h>

#define CONNECTED 1
#define NOT_CONNECTED 0

typedef struct {
    // Is the esp connected to a Wi-Fi
    int WIFIStatus;
    // is the esp connected to a server?
    int TCPStatus;
    // is the board connected to an MQTT Broker
    int MQTTStatus;

} NetworkStatusFlags;
NetworkStatusFlags Network_GetStatusFlags();

bool Network_Init(bool softInit);

typedef struct {
    // the name of your WI-FI
    char *ssid;
    // the Password for your WI-FI
    char *password;
} NetworkCredentials;
bool Network_ConnectToNetwork(NetworkCredentials credentials);
bool Network_DisconnectFromNetwork();

bool Network_Ping(char *target);

bool Network_TCP_Open(char *target, int port);
bool Network_TCP_Close(bool force);
bool Network_TCP_SendData(char *data, int timeoutMs);
bool Network_TCP_IsResponseAvailable();
// free response after use!
char *Network_TCP_GetResponse();

typedef struct {
    char *topic;
    char *data;
} MQTT_Message;
bool Network_MQTT_SetClientId(char *clientId);
bool Network_MQTT_Open(char *host, int port);
bool Network_MQTT_Close(bool force);
bool Network_MQTT_Publish(MQTT_Message message);
bool Network_MQTT_Subscribe(char *topic);
bool Network_MQTT_Unsubscribe(char *topic);
bool Network_MQTT_IsResponseAvailable();
// free response.topic and .data after use
bool Network_MQTT_GetResponse(MQTT_Message *response);

#endif
