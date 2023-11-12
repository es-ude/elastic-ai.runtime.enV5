#define SOURCE_FILE "TEST-HELPER"

#include "HardwaretestHelper.h"
#include "Esp.h"
#include "FreeRtosQueueWrapper.h"
#include "FreeRtosTaskWrapper.h"
#include "MqttBroker.h"
#include "Network.h"

#include "hardware/watchdog.h"
#include "pico/bootrom.h"
#include "pico/stdlib.h"

networkCredentials_t networkCredentials = {.ssid = "SSID", .password = "password"};
mqttBrokerHost_t mqttHost = {.ip = "0.0.0.0", .port = "1883", .userID = "", .password = ""};

void connectToNetwork(void) {
    networkTryToConnectToNetworkUntilSuccessful(networkCredentials);
}

void connectToMQTT(void) {
    mqttBrokerConnectToBrokerUntilSuccessful(mqttHost, "eip://uni-due.de/es", "enV5");
}

void initHardwareTest(void) {
    // Did we crash last time -> reboot into boot rom mode
    if (watchdog_enable_caused_reboot()) {
        reset_usb_boot(0, 0);
    }
    // init stdio and esp
    stdio_init_all();
    while ((!stdio_usb_connected())) {}
    espInit();
}

void _Noreturn enterBootModeTaskHardwareTest(void) {
    watchdog_enable(5000000, 1); // max timeout ~8.3s

    while (1) {
        if (getchar_timeout_us(0) == 'r' || !stdio_usb_connected()) {
            reset_usb_boot(0, 0);
        }
        watchdog_update();
        freeRtosTaskWrapperTaskSleep(500);
    }
}
