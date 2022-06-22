#define SOURCE_FILE "TEST-HELPER"

#include "hardwareTestHelper.h"
#include "configuration.h"
#include "pico/bootrom.h"
#include "pico/stdlib.h"
#include "hardware/watchdog.h"
#include "TaskWrapper.h"
#include "Network.h"
#include "MQTTBroker.h"
#include "QueueWrapper.h"
#include "esp.h"

void connectToNetwork(void) {
    Network_ConnectToNetworkUntilConnected(NetworkCredentials);
}

void connectToMQTT(void) {
    MQTT_Broker_ConnectToBrokerUntilConnected(MQTTHost, "eip://uni-due.de/es", "enV5");
}

void initHardwareTest(void) {
    // Did we crash last time -> reboot into boot rom mode
    if (watchdog_enable_caused_reboot()) {
        reset_usb_boot(0, 0);
    }
    // init usb, queue and watchdog
    stdio_init_all();
    while ((!stdio_usb_connected())) {}
    ESP_Init();
    CreateQueue();
    watchdog_enable(2000, 1);
}

void _Noreturn enterBootModeTaskHardwareTest(void) {
    while (true) {
        if (getchar_timeout_us(10) == 'r' || !stdio_usb_connected()) {
            reset_usb_boot(0, 0);
        }
        watchdog_update();
        TaskSleep(1000);
    }
}
