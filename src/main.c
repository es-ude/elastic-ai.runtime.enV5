#define SOURCE_FILE "MAIN"
// src headers
#include "MQTTBroker.h"
#include "Network.h"
#include "QueueWrapper.h"
#include "TaskWrapper.h"
#include "common.h"
#include "configuration.h"
#include "esp.h"
// external headers
#include "hardware/watchdog.h"
#include "pico/bootrom.h"
#include "pico/stdlib.h"

_Noreturn void mainTask(void) {
    Network_ConnectToNetworkUntilConnected(NetworkCredentials);
    MQTT_Broker_ConnectToBrokerUntilConnected(MQTTHost, "eip://uni-due.de/es", "enV5");

    while (true) {
        PRINT("Hello, World!")
        TaskSleep(5000);
    }
}

// Goes into bootloader mod load when 'r' is pressed
_Noreturn void enterBootModeTask(void) {
    while (true) {
        if (getchar_timeout_us(10) == 'r' || !stdio_usb_connected()) {
            reset_usb_boot(0, 0);
        }
        // Watchdog update needs to be performed frequent, otherwise the device
        // will crash
        watchdog_update();
        TaskSleep(1000);
    }
}

void init(void) {
    // First check if we crash last time -> reboot into boot rom mode
    if (watchdog_enable_caused_reboot()) {
        reset_usb_boot(0, 0);
    }
    // init usb, queue and watchdog
    stdio_init_all();
    // waits for usb connection, REMOVE to continue without waiting for
    // connection
    while ((!stdio_usb_connected()))
        ;
    // Checks connection to ESP and initializes
    ESP_Init();
    // Create FreeRTOS task queue
    CreateQueue();
    // enables watchdog to check for reboots
    watchdog_enable(2000, 1);
}

int main() {
    init();
    RegisterTask(enterBootModeTask, "enterBootModeTask");
    RegisterTask(mainTask, "mainTask");
    // Starts FreeRTOS tasks
    StartScheduler();
}
