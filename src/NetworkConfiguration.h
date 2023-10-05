#ifndef ENV5_NETWORK_CONFIGURATION
#define ENV5_NETWORK_CONFIGURATION

#include "MqttBroker.h"
#include "Network.h"

networkCredentials_t networkCredentials = {.ssid = "SSID", .password = "password"};

mqttBrokerHost_t mqttHost = {.ip = "0.0.0.0", .port = "1883", .userID = "", .password = ""};

#endif /* ENV5_NETWORK_CONFIGURATION */
