#ifndef ENV5_NETWORK_CONFIGURATION
#define ENV5_NETWORK_CONFIGURATION

#include "MqttBroker.h"
#include "Network.h"

/**
 * Be aware that special characters ned
 *
 */
NetworkCredentials_t NetworkCredentials = {.ssid = "FRITZ!Box_Cable_Federl",
                                           .password = "oLJ\\\"xhhF5j_$#8}z+ST{"};

MQTTHost_t MQTTHost = {.ip = "192.168.178.24", .port = "1883", .userID = "", .password = ""};

#endif /* ENV5_NETWORK_CONFIGURATION */
