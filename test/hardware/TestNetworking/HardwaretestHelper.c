#define SOURCE_FILE "TEST-HELPER"

#include "HardwaretestHelper.h"
#include "EnV5HwController.h"
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

void connectToMqttBroker(void) {
    mqttBrokerConnectToBrokerUntilSuccessful("eip://uni-due.de/es", "enV5");
}

void initHardwareTest(void) {
    // check if we crash last time -> reboot into boot rom mode
    if (watchdog_enable_caused_reboot()) {
        reset_usb_boot(0, 0);
    }

    env5HwInit();

    // initialize the serial output
    stdio_init_all();
    while ((!stdio_usb_connected())) {
        // wait for serial connection
    }

    espInit();
}

void _Noreturn enterBootModeTask(void) {
    watchdog_enable(5000, 1); // 5 seconds

    while (1) {
        if (getchar_timeout_us(0) == 'r' || !stdio_usb_connected()) {
            reset_usb_boot(0, 0);
        }
        watchdog_update();
        freeRtosTaskWrapperTaskSleep(1500);
    }
}
