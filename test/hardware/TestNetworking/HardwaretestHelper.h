#ifndef SENSOR_BOARD_HARDWARE_TEST_HELPER_H
#define SENSOR_BOARD_HARDWARE_TEST_HELPER_H

void connectToNetwork(void);

void connectToMqttBroker(void);

void initHardwareTest(void);

_Noreturn void enterBootModeTask(void);

#endif // SENSOR_BOARD_HARDWARE_TEST_HELPER_H
