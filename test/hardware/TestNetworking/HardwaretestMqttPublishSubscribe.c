#define SOURCE_FILE "MQTT-PUBLISH/SUBSCRIBE-TEST"

#include "Common.h"
#include "FreeRtosTaskWrapper.h"
#include "HardwaretestHelper.h"
#include "MqttBroker.h"
#include "Protocol.h"
#include <malloc.h>
#include <stdio.h>
#include <string.h>

/*!
 * Connects to Wi-Fi and MQTT Broker (Change in src/configuration.h).
 * Subscribes and publishes to topic "eip://uni-due.de/es/test" and prints the received Data.
 */

void publishTestData(uint16_t i) {
    char buffer[2];
    sprintf(buffer, "%d", i);
    char *data = malloc(strlen("testData") + strlen(buffer));
    strcpy(data, "testData");
    strcat(data, buffer);
    protocolPublishData("testPubSub", data);
    free(data);
}

void deliver(posting_t posting) {
    PRINT("Received Data: %s", posting.data)
}

_Noreturn void mqttTask(void) {
    PRINT("=== STARTING TEST ===")

    connectToNetwork();
    connectToMQTT();

    protocolSubscribeForData("enV5", "testPubSub", (subscriber_t){.deliver = deliver});

    uint64_t messageCounter = 0;
    while (1) {
        publishTestData(messageCounter);
        messageCounter++;
        TaskSleep(1000);
    }
}

int main() {
    initHardwareTest();

    RegisterTask(enterBootModeTaskHardwareTest, "enterBootModeTask");
    RegisterTask(mqttTask, "mqttTask");
    StartScheduler();
}
