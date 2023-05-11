#ifndef ENV5_NETWORK_CONFIGURATION
#define ENV5_NETWORK_CONFIGURATION

#include "MqttBroker.h"
#include "Network.h"

networkCredentials_t networkCredentials = {.ssid = "ssid", .password = "passwort"};

mqttBrokerHost_t mqttHost = {
    .ip = "255.255.255.255", .port = "1883", .userID = "user", .password = "pwd"};

#endif /* ENV5_NETWORK_CONFIGURATION */
