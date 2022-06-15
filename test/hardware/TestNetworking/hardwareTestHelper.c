#define SOURCE_FILE "TEST-HELPER"

#include "hardwareTestHelper.h"
#include "MQTTBroker.h"
#include "Network.h"
#include "NetworkSettings.h"
#include "QueueWrapper.h"
#include "TaskWrapper.h"
#include "esp.h"
#include "hardware/watchdog.h"
#include "pico/bootrom.h"
#include "pico/stdlib.h"

void connectToNetwork(void) {
    Network_ConnectToNetworkUntilConnected(credentials);
}

void connectToMQTT(void) {
    MQTT_Broker_ConnectToBrokerUntilConnected(mqttHost, "1883",
                                              "eip://uni-due.de/es", "enV5");
}

void initHardwareTest(void) {
    // Did we crash last time -> reboot into boot rom mode
    if (watchdog_enable_caused_reboot()) {
        reset_usb_boot(0, 0);
    }
    // init usb, queue and watchdog
    stdio_init_all();
    while ((!stdio_usb_connected())) {
    }
    ESP_Init();
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
