#ifndef ENV5_CONFIGURATION_H
#define ENV5_CONFIGURATION_H

#include "MQTTBroker.h"
#include "Network.h"

NetworkCredentials_t NetworkCredentials = {.ssid = "ES-Stud", .password = "curjeq343j"};

MQTTHost_t MQTTHost = {.ip = "192.168.203.50", .port = "1883"};

#endif // ENV5_CONFIGURATION_H
