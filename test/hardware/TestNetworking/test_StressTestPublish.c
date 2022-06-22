#define SOURCE_FILE "MQTT-STRESSTEST-PUBLISH"

#include "MQTTBroker.h"
#include "TaskWrapper.h"
#include "common.h"
#include "hardwareTestHelper.h"
#include "protocol.h"
#include <malloc.h>
#include <stdio.h>
#include <string.h>

/*!
 * Similar to test_MQTTPublish.c. Connects to Wi-Fi and MQTT Broker (Change in src/configuration.h).
 * When connected it publishes as fast as possible to eip://uni-due.de/es/stresstest.
 */

void publishTestData(uint64_t i) {
    char buffer[8];
    sprintf(buffer, "%llu", i);
    char *data = malloc(strlen("testData") + strlen(buffer) + 1);
    strcpy(data, "stress: ");
    strcat(data, buffer);
    publishData("stresstestPub", data);
    free(data);
}

void _Noreturn mqttTask(void) {
    PRINT("=== STARTING TEST ===")

    connectToNetwork();
    connectToMQTT();

    PRINT("Start sending...")

    uint64_t i = 0;
    while (true) {
        PRINT("stress: %llu", i)
        i++;
        publishTestData(i);
    }
}

int main() {
    initHardwareTest();
    RegisterTask(enterBootModeTaskHardwareTest, "enterBootModeTask");
    RegisterTask(mqttTask, "mqttTask");
    StartScheduler();
}
