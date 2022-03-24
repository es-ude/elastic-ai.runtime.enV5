#define SOURCE_FILE "MQTT Publish/Subscribe Test"

#include <stdio.h>

#include "TaskWrapper.h"

#include "Network.h"

#include "communicationEndpoint.h"
#include "espBroker.h"

#include <string.h>
#include "malloc.h"
#include "common.h"

#include "hardwareTestHelper.h"

/***
    Connects to Wi-Fi and MQTT Broker (Change in NetworkSettings.h).
    Subscribes and publishes to topic "eip://uni-due.de/es/test" and prints the received Data.
***/

void publishTestData(uint16_t i);

void deliver(Posting posting) {
    PRINT("RECEIVED: %s", posting.data)
}

void _Noreturn mqttTask(void) {
    setID("eip://uni-due.de/es");
    ESP_MQTT_SetClientId("ENV5");

    connectToNetworkAndMQTT();

    subscribe("test", (Subscriber) {.deliver=deliver});

    for (uint16_t i = 0; i < 65536; ++i) {
        connectToNetworkAndMQTT();
        publishTestData(i);
        TaskSleep(1000);
    }
}

void publishTestData(uint16_t i) {
    char buffer[2];
    sprintf(buffer, "%d", i);
    char *data = malloc(strlen("testData") + strlen(buffer));
    strcpy(data, "testData");
    strcat(data, buffer);
    publish((Posting) {.topic="test", .data=data});
    free(data);
}

int main() {
    initHardwareTest();

    RegisterTask(enterBootModeTaskHardwareTest, "enterBootModeTask");
    RegisterTask(mqttTask, "mqttTask");
    StartScheduler();
}
