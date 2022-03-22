#include <stdio.h>

#include "TaskWrapper.h"
#include "QueueWrapper.h"

#include "Network.h"
#include "pico/bootrom.h"
#include "pico/stdlib.h"
#include "hardware/watchdog.h"

#include "communicationEndpoint.h"
#include "espBroker.h"

/* FIXME: update wifi credentials and MQTT Host IP/Port */
NetworkCredentials credentials = {
        .ssid     = "wifi_network",
        .password = "wifi_password"};

void enterBootModeTask(void);

void init(void);

int main() {
    init();

    printf("Main: Hello, world!\n");

    RegisterTask(enterBootModeTask, "enterBootModeTask");
    StartScheduler();

    publish((Posting) {.topic="test", .data="testData"});
}

void init(void) {
    // Did we crash last time -> reboot into bootrom mode
    if (watchdog_enable_caused_reboot()) {
        Network_Init(true);
        initBroker("123.446", "8080", "DOMAIN");
//        Network_MQTT_Close(true);
        reset_usb_boot(0, 0);
    }
    // init usb, queue and watchdog
    stdio_init_all();
    while ((!stdio_usb_connected())) {}
    CreateQueue();
    watchdog_enable(2000, 1);
}

void enterBootModeTask(void) {
    while (true) {
        if (getchar_timeout_us(10) == 'r' || !stdio_usb_connected()) {
            ESP_MQTT_Disconnect(true);
            reset_usb_boot(0, 0);
        }
        watchdog_update();
        TaskSleep(1000);
    }
}
