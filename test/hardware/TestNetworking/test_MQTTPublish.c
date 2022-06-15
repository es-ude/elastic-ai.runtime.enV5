#define SOURCE_FILE "MQTT-PUBLISH-TEST"

#include "MQTTBroker.h"
#include "Network.h"
#include "TaskWrapper.h"
#include "common.h"
#include "hardwareTestHelper.h"
#include "protocol.h"
#include <malloc.h>
#include <stdio.h>
#include <string.h>

/***
    Connects to Wi-Fi and MQTT Broker (Change in NetworkSettings.h).
    When connected publishes each second "testData" plus incrementing integer to
eip://uni-due.de/es/DATA/test. Can be received with the Java Integration Test
IntegrationTestWhereENv5IsPublishing.
***/

void publishTestData(uint16_t i) {
    char buffer[2];
    sprintf(buffer, "%d", i);
    char *data = malloc(strlen("testData") + strlen(buffer));
    strcpy(data, "testData");
    strcat(data, buffer);
    publishData("testPub", data);
    free(data);
}

void _Noreturn mqttTask(void) {
    PRINT("Starting Test")

    uint64_t i = 0;
    while (true) {
        connectToNetwork();
        connectToMQTT();

        publishTestData(i);
        i++;
        TaskSleep(1000);
    }
}

int main() {
    initHardwareTest();

    RegisterTask(enterBootModeTaskHardwareTest, "enterBootModeTask");
    RegisterTask(mqttTask, "mqttTask");
    StartScheduler();
}
