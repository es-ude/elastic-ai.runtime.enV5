#define SOURCE_FILE "MQTT-STRESSTEST-PUBLISH"

#include "hardwareTestHelper.h"
#include "TaskWrapper.h"
#include "MQTTBroker.h"
#include "common.h"
#include "protocol.h"
#include <stdio.h>
#include <string.h>
#include <malloc.h>

/***
    Similar to test_MQTTPublish.c
    Connects to Wi-Fi and MQTT Broker (Change in NetworkSettings.h).
    When connected it publishes as fast as possible to eip://uni-due.de/es/stresstest.
***/

void publishTestData(uint16_t i);

void _Noreturn mqttTask(void) {
    connectToNetwork();
    connectToMQTT();

    PRINT("Start sending...")

    uint64_t i = 0;
    while (true) {
        PRINT("stress: %llu", i)
        publishTestData(i);
        i++;
    }
}

void publishTestData(uint16_t i) {
    char buffer[2];
    sprintf(buffer, "%d", i);
    char *data = malloc(strlen("testData") + strlen(buffer));
    strcpy(data, "stress: ");
    strcat(data, buffer);
    publishData("stresstestPub", data);
    free(data);
}

int main() {
    initHardwareTest();
    RegisterTask(mqttTask, "mqttTask");
    StartScheduler();
}
