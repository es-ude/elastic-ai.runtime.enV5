#define SOURCE_FILE "MQTT-PUBLISH-TEST"

#include "Common.h"
#include "FreeRtosTaskWrapper.h"
#include "HardwaretestHelper.h"
#include "MqttBroker.h"
#include "Protocol.h"
#include <malloc.h>
#include <stdio.h>
#include <string.h>

/*!
 * Connects to Wi-Fi and MQTT Broker (Change in src/configuration.h). When connected publishes each
 * second "testData" plus incrementing integer to eip://uni-due.de/es/DATA/test. Can be received
 * with the Java Integration TestIntegrationTestWhereENv5IsPublishing.
 */

void publishTestData(uint16_t i) {
    char buffer[2];
    sprintf(buffer, "%d", i);
    char *data = malloc(strlen("testData") + strlen(buffer));
    strcpy(data, "testData");
    strcat(data, buffer);
    protocolPublishData("testPub", data);
    free(data);
}

void _Noreturn mqttTask(void) {
    PRINT("=== STARTING TEST ===")

    connectToNetwork();
    connectToMQTT();

    uint64_t messageCounter = 0;
    while (1) {
        publishTestData(messageCounter);
        messageCounter++;
        freeRtosTaskWrapperTaskSleep(1000);
    }
}

int main() {
    initHardwareTest();

    freeRtosTaskWrapperRegisterTask(enterBootModeTaskHardwareTest, "enterBootModeTask", 0);
    freeRtosTaskWrapperRegisterTask(mqttTask, "mqttTask", 0);
    freeRtosTaskWrapperStartScheduler();
}
