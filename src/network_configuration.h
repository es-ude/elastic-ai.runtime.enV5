#ifndef ENV5_NETWORK_CONFIGURATION
#define ENV5_NETWORK_CONFIGURATION

#include "MQTTBroker.h"
#include "Network.h"

/**
 * Be aware that special characters ned
 *
 */
NetworkCredentials_t NetworkCredentials = {.ssid = "ES-Stud", .password = "curjeq343j"};

MQTTHost_t MQTTHost = {.ip = "127.0.0.1", .port = "1883", .userID = "", .password = ""};

#endif /* ENV5_NETWORK_CONFIGURATION */
