#ifndef ENV5_CONFIGURATION_H
#define ENV5_CONFIGURATION_H

#include "Network.h"
#include "MQTTBroker.h"

NetworkCredentials_t NetworkCredentials = {
        .ssid     = "SSID",
        .password = "password"};

MQTTHost_t MQTTHost = {
        .ip     = "1.2.3.4",
        .port = "1883"};

#endif //ENV5_CONFIGURATION_H
