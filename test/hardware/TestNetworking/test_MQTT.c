#include "TaskWrapper.h"
#include "QueueWrapper.h"
#include "esp/espBase.h"
#include "esp/espMQTT.h"
#include "pico/bootrom.h"
#include "pico/stdlib.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "hardware/watchdog.h"

/*
 * If everything works this should periodically output random numbers to the stdout
 */

/* FIXME: update wifi credentials and MQTT Host IP/Port */
ESP_NetworkCredentials credentials = {
    .ssid     = "wifi_network",
    .password = "wifi_password"};
char *mqttHost = "192.168.178.1";
char *mqttPort="1883";
char *topic="test";

void TestEnterBootModeTask();
void MQTTTask();
void MQTTReceiverTask();

int main() {
    // Did we crash last time -> reboot into bootrom mode
    if (watchdog_enable_caused_reboot()) {
        ESP_Init(true);
        ESP_MQTT_Disconnect(true);
        reset_usb_boot(0, 0);
    }
    // init usb, random, queue and watchdog
    stdio_init_all();
    while ((!stdio_usb_connected())) {}
    char seed[9];
    strcpy(seed, __TIME__);
    seed[2] = '0';
    seed[5] = '0';
    srand(atoi(seed));
    CreateQueue();
    watchdog_enable(2000, 1);

    printf("test_MQTT:\n");
    RegisterTask(MQTTTask, "MQTTTask");
    RegisterTask(TestEnterBootModeTask, "TestEnterBootModeTask");
    StartScheduler();
}

_Noreturn void MQTTTask() {
    ESP_Init(false);

    while (ESP_GetStatusFlags().WIFIStatus == NOT_CONNECTED)
        ESP_ConnectToNetwork(credentials);

    bool subscribed = false;
    while (true) {
        if (ESP_GetStatusFlags().MQTTStatus == CONNECTED) {
            if (!subscribed) {
                if(ESP_MQTT_Subscribe(topic)) {
                    subscribed = true;
                    RegisterTask(MQTTReceiverTask, "MQTTRec");
                }
            } else {
                char buf[10];
                sprintf(buf, "random%d", rand());
                ESP_MQTT_Message msg = {
                        .topic = topic,
                        .data =  buf
                };
                while (ESP_MQTT_IsResponseAvailable())
                    TaskSleep(100);
                ESP_MQTT_Publish(msg);
            }
        } else {
            printf("Connecting to Broker...\n");
            ESP_MQTT_SetClientId("esdevice");
            if (ESP_MQTT_ConnectToBroker(mqttHost, mqttPort))
                printf("Connected\n");
            else
                ESP_MQTT_Disconnect(true);
        }
        TaskSleep(2000);
    }
}

_Noreturn void MQTTReceiverTask() {
    while (true) {
        while (ESP_MQTT_IsResponseAvailable()) {
            ESP_MQTT_Message response = {};
            if (ESP_MQTT_GetResponse(&response)) {
                printf("Response available: %s: %s\n", response.topic, response.data);
                free(response.topic);
                free(response.data);
            }
        }
        printf("No response available\n");
        TaskSleep(1000);
    }
}

void TestEnterBootModeTask() {
    while (true) {
        if (getchar_timeout_us(10) == 'r' || !stdio_usb_connected()) {
            ESP_MQTT_Disconnect(true);
            reset_usb_boot(0, 0);
        }
        watchdog_update();
        TaskSleep(1000);
    }
}
