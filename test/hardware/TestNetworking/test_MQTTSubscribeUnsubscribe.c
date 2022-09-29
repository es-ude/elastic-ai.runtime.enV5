#define SOURCE_FILE "MQTT-PUBLISH/SUBSCRIBE-TEST"

#include "MQTTBroker.h"
#include "Network.h"
#include "TaskWrapper.h"
#include "common.h"
#include "hardwareTestHelper.h"
#include "Protocol.h"
#include <malloc.h>
#include <stdio.h>
#include <string.h>

/*!
 * Connects to Wi-Fi and MQTT Broker (Change in src/configuration.h). Subscribes and publishes to
 * topic "eip://uni-due.de/es/test" and prints the received Data.
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

    subscriber_t sub = (subscriber_t ){.deliver = deliver};

    uint64_t i = 0;
    while (true) {
        protocolSubscribeForData("enV5", "testPubSub", sub);
        PRINT("Should receive data with id: %llu", i)
        publishTestData(i);
        i++;
        TaskSleep(2500);

        protocolUnsubscribeFromData("enV5", "testPubSub", sub);
        PRINT("Should NOT receive data with id: %llu", i)
        publishTestData(i);
        i++;
        TaskSleep(2500);
    }
}

int main() {
    initHardwareTest();

    RegisterTask(enterBootModeTaskHardwareTest, "enterBootModeTask");
    RegisterTask(mqttTask, "mqttTask");
    StartScheduler();
}
