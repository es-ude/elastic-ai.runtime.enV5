#ifndef ENV5_CONFIGURATION_H
#define ENV5_CONFIGURATION_H

#include "Network.h"

NetworkCredentials_t NetworkCredentials = {.ssid = "SSID",
                                           .password = "password"};

// IP address of MQTT Broker
char *mqttHost = "1.2.3.4";

#endif // ENV5_CONFIGURATION_H
