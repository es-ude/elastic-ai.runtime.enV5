#ifndef NETWORK_H
#define NETWORK_H

#include <stdbool.h>
#include <stdint.h>

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
} NetworkStatus_t;

typedef struct {
    // the name of your WI-FI
    char *ssid;
    // the Password for your WI-FI
    char *password;
} NetworkCredentials;

extern NetworkStatus_t NetworkStatus;

bool Network_init(void);

bool Network_ConnectToNetwork(NetworkCredentials credentials);

void Network_ConnectToNetworkPlain(char *ssid, char *password);

void Network_DisconnectFromNetwork(void);

#endif
