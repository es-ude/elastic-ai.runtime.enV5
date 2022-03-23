#define SOURCE_FILE "MAIN"

#include <stdio.h>

#include "TaskWrapper.h"
#include "QueueWrapper.h"

#include "Network.h"
#include "pico/bootrom.h"
#include "pico/stdlib.h"
#include "hardware/watchdog.h"
#include "esp/espBase.h"
#include "communicationEndpoint.h"
#include "espBroker.h"

#include <string.h>
#include "common.h"
#include "malloc.h"

/* FIXME: update wifi credentials and MQTT Host IP/Port */
NetworkCredentials credentials = {
        .ssid     = "SSID",
        .password = "password"};

char *mqttHost = "123.456.789.123";

void enterBootModeTask(void);

void init(void);

void mqttTask(void) {
    Network_ConnectToNetwork(credentials);

    setBrokerID("eip://uni-due.de/es/");
    ESP_MQTT_SetClientId("ENV5");

    while (ESP_GetStatusFlags().MQTTStatus == NOT_CONNECTED) {
        ESP_MQTT_ConnectToBroker(mqttHost, "1883");
        TaskSleep(2000);
    }

    for (uint16_t i = 0; i < 10000; ++i) {
        char buffer[2];
        sprintf(buffer, "%d", i);
        char *data = malloc(strlen("testData") + strlen(buffer));
        strcpy(data, "testData");
        strcat(data, buffer);
        publish((Posting) {.topic="test", .data=data});
        free(data);
        TaskSleep(1000);
    }
}

int main() {
    init();

    RegisterTask(enterBootModeTask, "enterBootModeTask");
    RegisterTask(mqttTask, "mqttTask");
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
