#define SOURCE_FILE "TEST-HELPER"

#include "hardwareTestHelper.h"

#include "Network.h"
#include "MQTTBroker.h"

#include "TaskWrapper.h"
#include "esp.h"

#include "pico/bootrom.h"
#include "pico/stdlib.h"
#include "hardware/watchdog.h"
#include "QueueWrapper.h"

#include "NetworkSettings.h"

#include "common.h"

void connectToNetwork(void) {
    while (NetworkStatus.WIFIStatus == NOT_CONNECTED) {
        Network_ConnectToNetwork(credentials);
        TaskSleep(500);
    }
}

void connectToMQTT(void) {
    while (NetworkStatus.MQTTStatus == NOT_CONNECTED) {
        MQTT_Broker_ConnectToBroker(mqttHost, "1883");
        TaskSleep(500);
    }
}

void initHardwareTest(void) {
    // Did we crash last time -> reboot into boot rom mode
    if (watchdog_enable_caused_reboot()) {
        reset_usb_boot(0, 0);
    }
    Network_init(false);
    // init usb, queue and watchdog
    stdio_init_all();
    while ((!stdio_usb_connected())) {}
    CreateQueue();
    watchdog_enable(2000, 1);
}

void _Noreturn enterBootModeTaskHardwareTest(void) {
    while (true) {
        if (getchar_timeout_us(10) == 'r' || !stdio_usb_connected()) {
            MQTT_Broker_Disconnect(true);
            reset_usb_boot(0, 0);
        }
        watchdog_update();
        TaskSleep(1000);
    }
}
