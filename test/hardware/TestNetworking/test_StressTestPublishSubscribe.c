#define SOURCE_FILE "MQTT-STRESSTEST-PUBLISH/SUBSCRIBE"

#include "MQTTBroker.h"
#include "Network.h"
#include "TaskWrapper.h"
#include "common.h"
#include "hardwareTestHelper.h"
#include "protocol.h"
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
    publishData("stresstestPubSub", data);
    free(data);
}

void deliver(Posting posting) {
    PRINT("Received Data: %s", posting.data)
}

_Noreturn void mqttTask(void) {
    PRINT("=== STARTING TEST ===")

    connectToNetwork();
    connectToMQTT();

    subscribeForData("enV5", "stresstestPubSub", (Subscriber){.deliver = deliver});

    uint64_t i = 0;
    while (true) {
        publishTestData(i);
        i++;
    }
}

int main() {
    initHardwareTest();
    RegisterTask(enterBootModeTaskHardwareTest, "enterBootModeTask");
    RegisterTask(mqttTask, "mqttTask");
    StartScheduler();
}
