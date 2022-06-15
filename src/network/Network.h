#ifndef NETWORK_H
#define NETWORK_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    char *ssid;
    char *password;
} NetworkCredentials_t;

void Network_ConnectToNetworkUntilConnected(NetworkCredentials_t credentials);

bool Network_ConnectToNetwork(NetworkCredentials_t credentials);

void Network_DisconnectFromNetwork(void);

#endif
