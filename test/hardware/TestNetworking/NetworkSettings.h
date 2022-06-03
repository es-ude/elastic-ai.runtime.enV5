#ifndef SENSOR_BOARD_NETWORK_SETTINGS_H
#define SENSOR_BOARD_NETWORK_SETTINGS_H

#include "Network.h"

/* FIXME: update wifi credentials */
NetworkCredentials credentials = {
        .ssid     = "ES-Stud",
        .password = "curjeq343j"};

/* FIXME: Set IP address of MQTT Host */
char *mqttHost = "192.168.203.46";

#endif //SENSOR_BOARD_NETWORK_SETTINGS_H
