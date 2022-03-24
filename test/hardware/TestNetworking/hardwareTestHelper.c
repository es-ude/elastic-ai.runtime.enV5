#include "hardwareTestHelper.h"

#include "Network.h"
#include "espMQTTBroker.h"

#include "TaskWrapper.h"
#include "espBase.h"

#include "pico/bootrom.h"
#include "pico/stdlib.h"
#include "hardware/watchdog.h"
#include "QueueWrapper.h"

#include "NetworkSettings.h"

void connectToNetworkAndMQTT() {
    while (ESP_GetStatusFlags().WIFIStatus == NOT_CONNECTED) {
        Network_ConnectToNetwork(credentials);
        TaskSleep(100);
    }
    while (ESP_GetStatusFlags().MQTTStatus == NOT_CONNECTED) {
        ESP_MQTT_BROKER_ConnectToBroker(mqttHost, "1883");
        TaskSleep(100);
    }
}

void initHardwareTest(void) {
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

void _Noreturn enterBootModeTaskHardwareTest(void) {
    while (true) {
        if (getchar_timeout_us(10) == 'r' || !stdio_usb_connected()) {
            ESP_MQTT_BROKER_Disconnect(true);
            reset_usb_boot(0, 0);
        }
        watchdog_update();
        TaskSleep(1000);
    }
}

