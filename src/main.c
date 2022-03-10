#include <stdio.h>

#include "TaskWrapper.h"
#include "QueueWrapper.h"

#include "Network.h"
#include "pico/bootrom.h"
#include "pico/stdlib.h"
#include "hardware/watchdog.h"

/* FIXME: update wifi credentials and MQTT Host IP/Port */
NetworkCredentials credentials = {
        .ssid     = "wifi_network",
        .password = "wifi_password"};

void TestEnterBootModeTask(void);

void init(void);

int main() {
    init();

    printf("Main: Hello, world!\n");

    RegisterTask(TestEnterBootModeTask, "TestEnterBootModeTask");
    StartScheduler();
}

void init(void) {
    // Did we crash last time -> reboot into bootrom mode
    if (watchdog_enable_caused_reboot()) {
        Network_Init(true);
        Network_MQTT_Close(true);
        reset_usb_boot(0, 0);
    }
    // init usb, queue and watchdog
    stdio_init_all();
    while ((!stdio_usb_connected())) {}
    CreateQueue();
    watchdog_enable(2000, 1);
}

void TestEnterBootModeTask(void) {
    while (true) {
        if (getchar_timeout_us(10) == 'r' || !stdio_usb_connected()) {
            Network_MQTT_Close(true);
            reset_usb_boot(0, 0);
        }
        watchdog_update();
        TaskSleep(1000);
    }
}
