#define SOURCE_FILE "TEST-HELPER"

#include "include/HardwaretestHelper.h"
#include "Esp.h"
#include "FreeRtosTaskWrapper.h"
#include "MqttBroker.h"
#include "Network.h"

#include "hardware/watchdog.h"
#include "pico/bootrom.h"
#include "pico/stdlib.h"

void connectToNetwork(void) {
    networkTryToConnectToNetworkUntilSuccessful();
}

void connectToMQTT(void) {
    mqttBrokerConnectToBrokerUntilSuccessful("enV5", NULL);
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
