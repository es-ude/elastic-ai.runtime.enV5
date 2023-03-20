#ifndef ENV5_NETWORK_CONFIGURATION
#define ENV5_NETWORK_CONFIGURATION

#include "MqttBroker.h"
#include "Network.h"

networkCredentials_t networkCredentials = {.ssid = "iPhone von David", .password = "gert2244"};

mqttBrokerHost_t mqttHost = {.ip = "172.20.10.7", .port = "1883", .userID = "", .password = ""};

#endif /* ENV5_NETWORK_CONFIGURATION */
