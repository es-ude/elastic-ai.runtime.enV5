#define SOURCE_FILE "MAIN"
#define NDEBUG

#include <stdio.h>

#include "TaskWrapper.h"
#include "QueueWrapper.h"

#include "Network.h"
#include "pico/bootrom.h"
#include "pico/stdlib.h"
#include "hardware/watchdog.h"
#include "espMQTTBroker.h"

#include "common.h"

void enterBootModeTask(void);

void init(void);

void mainTask(void) {
    while (true) {
        PRINT("Hello, World!")
        TaskSleep(1000);
    }
}

int main() {
    init();

    RegisterTask(enterBootModeTask, "enterBootModeTask");
    RegisterTask((TaskCodeFunc) mainTask, "mainTask");
    StartScheduler();
}

void init(void) {
    // Did we crash last time -> reboot into bootrom mode
    if (watchdog_enable_caused_reboot()) {
        reset_usb_boot(0, 0);
    }
    Network_Init(false);
    // init usb, queue and watchdog
    stdio_init_all();
    while ((!stdio_usb_connected())) {}
    CreateQueue();
    watchdog_enable(2000, 1);
}

void _Noreturn enterBootModeTask(void) {
    while (true) {
        if (getchar_timeout_us(10) == 'r' || !stdio_usb_connected()) {
            ESP_MQTT_BROKER_Disconnect(true);
            PRINT("Enter boot mode...")
            reset_usb_boot(0, 0);
        }
        watchdog_update();
        TaskSleep(1000);
    }
}
