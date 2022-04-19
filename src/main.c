#define SOURCE_FILE "MAIN"
#define NDEBUG

#include <stdio.h>

#include "hardware/watchdog.h"
#include "pico/bootrom.h"
#include "pico/stdlib.h"

#include "common.h"
#include "MQTTBroker.h"
#include "QueueWrapper.h"
#include "TaskWrapper.h"
#include "Network.h"

#include "communicationEndpoint.h"

void enterBootModeTask(void);

void init(void);

void mainTask(void) {
    while (true) {
        PRINT("Hello, World!")
        TaskSleep(5000);
    }
}

int main() {
    init();

    RegisterTask(enterBootModeTask, "enterBootModeTask");
    RegisterTask((TaskCodeFunc) mainTask, "mainTask");
    StartScheduler();
}

void init(void) {
    // Did we crash last time -> reboot into boot rom mode
    if (watchdog_enable_caused_reboot()) {
        reset_usb_boot(0, 0);
    }
    while (!Network_init());
    // init usb, queue and watchdog
    stdio_init_all();
    while ((!stdio_usb_connected())) {}
    CreateQueue();
    watchdog_enable(2000, 1);
}

void _Noreturn enterBootModeTask(void) {
    while (true) {
        if (getchar_timeout_us(10) == 'r' || !stdio_usb_connected()) {
            MQTT_Broker_Disconnect(true);
            reset_usb_boot(0, 0);
        }
        watchdog_update();
        TaskSleep(1000);
    }
}
