#ifndef SENSOR_BOARD_NETWORK_SETTINGS_H
#define SENSOR_BOARD_NETWORK_SETTINGS_H

#include "Network.h"

/* FIXME: update wifi credentials */
NetworkCredentials_t credentials = {
        .ssid     = "SSID",
        .password = "password"};

/* FIXME: Set IP address of MQTT Host */
char *mqttHost = "1.2.3.4";

#endif //SENSOR_BOARD_NETWORK_SETTINGS_H
