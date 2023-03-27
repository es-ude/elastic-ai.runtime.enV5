#ifndef ENV5_NETWORK_CONFIGURATION
#define ENV5_NETWORK_CONFIGURATION

#include "MqttBroker.h"
#include "Network.h"

networkCredentials_t networkCredentials = {.ssid = "UPC0753630", .password = "KUvt6smejfvx"};

mqttBrokerHost_t mqttHost = {.ip = "192.168.0.61", .port = "1883", .userID = "", .password = ""};

#endif /* ENV5_NETWORK_CONFIGURATION */
