#ifndef SENSOR_BOARD_HARDWARETESTHELPER_H
#define SENSOR_BOARD_HARDWARETESTHELPER_H

void connectToNetworkAndMQTT();

void initHardwareTest(void);

void _Noreturn enterBootModeTaskHardwareTest(void);

#endif //SENSOR_BOARD_HARDWARETESTHELPER_H
