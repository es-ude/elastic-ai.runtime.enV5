#define SOURCE_FILE "MQTT-STRESSTEST-PUBLISH/SUBSCRIBE"

#include "Common.h"
#include "FreeRtosTaskWrapper.h"
#include "HardwaretestHelper.h"
#include "MqttBroker.h"
#include "Protocol.h"
#include <malloc.h>
#include <stdio.h>
#include <string.h>

/*!
 * Connects to Wi-Fi and MQTT Broker (Change in src/configuration.h). Subscribes and publishes to
 * topic "eip://uni-due.de/es/test" and prints the received Data.
 */

void publishTestData(uint64_t i) {
    char buffer[8];
    sprintf(buffer, "%llu", i);
    char *data = malloc(strlen("testData") + strlen(buffer) + 1);
    strcpy(data, "testData");
    strcat(data, buffer);
    protocolPublishData("stresstestPubSub", data);
    free(data);
}

void deliver(posting_t posting) {
    PRINT("Received Data: %s", posting.data)
}

_Noreturn void mqttTask(void) {
    PRINT("=== STARTING TEST ===")

    connectToNetwork();
    connectToMQTT();

    protocolSubscribeForData("enV5", "stresstestPubSub", (subscriber_t){.deliver = deliver});

    uint64_t messageCounter = 0;
    while (1) {
        publishTestData(messageCounter);
        messageCounter++;
    }
}

int main() {
    initHardwareTest();

    freeRtosTaskWrapperRegisterTask(enterBootModeTaskHardwareTest, "enterBootModeTask", 0);
    freeRtosTaskWrapperRegisterTask(mqttTask, "mqttTask", 0);
    freeRtosTaskWrapperStartScheduler();
}
