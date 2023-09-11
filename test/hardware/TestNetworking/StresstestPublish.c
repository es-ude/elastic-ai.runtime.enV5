#define SOURCE_FILE "MQTT-STRESSTEST-PUBLISH"

#include "Common.h"
#include "FreeRtosTaskWrapper.h"
#include "HardwaretestHelper.h"
#include "MqttBroker.h"
#include "Protocol.h"
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
    protocolPublishData("stresstestPub", data);
    free(data);
}

void _Noreturn mqttTask(void) {
    PRINT("=== STARTING TEST ===")

    connectToNetwork();
    connectToMQTT();

    PRINT("Start sending...")

    uint64_t messageCounter = 0;
    while (true) {
        PRINT("stress: %llu", messageCounter)
        messageCounter++;
        publishTestData(messageCounter);
    }
}

int main() {
    initHardwareTest();

    freeRtosTaskWrapperRegisterTask(enterBootModeTaskHardwareTest, "enterBootModeTask", 0, 0);
    freeRtosTaskWrapperRegisterTask(mqttTask, "mqttTask", 0, 0);
    freeRtosTaskWrapperStartScheduler();
}
