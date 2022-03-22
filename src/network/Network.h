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


typedef struct {
    // the name of your WI-FI
    char *ssid;
    // the Password for your WI-FI
    char *password;
} NetworkCredentials;


typedef struct {
    char *topic;
    char *data;
} MQTT_Message;

NetworkStatusFlags Network_GetStatusFlags();

bool Network_Init(bool softInit);

bool Network_ConnectToNetwork(NetworkCredentials credentials);

bool Network_DisconnectFromNetwork();

bool Network_Ping(char *ipAdress);

bool Network_TCP_Open(char *target, int port);

bool Network_TCP_Close(bool force);

bool Network_TCP_SendData(char *data, int timeoutMs);

bool Network_TCP_IsResponseAvailable();

char *Network_TCP_GetResponse();

#endif
