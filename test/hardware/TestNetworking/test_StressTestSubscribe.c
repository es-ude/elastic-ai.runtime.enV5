#define SOURCE_FILE "MQTT-STRESSTEST-PUBLISH"

#include "MQTTBroker.h"
#include "TaskWrapper.h"
#include "common.h"
#include "hardwareTestHelper.h"
#include "Protocol.h"
#include <stdio.h>
#include <string.h>

/*!
 * Similar to test_MQTTSubscribe.c. Connects to Wi-Fi and MQTT Broker (Change in
 * src/configuration.h). When connected it subscribes data published to
 * eip://uni-due.de/es/stresstest.
 */

void deliver(posting_t posting) {
    PRINT("Received Data: \"%s\", String length: \"%d\"", posting.data, strlen(posting.data))
}

_Noreturn void mqttTask(void) {
    PRINT("=== STARTING TEST ===")

    connectToNetwork();
    connectToMQTT();

    protocolSubscribeForData("integTestTwin", "testSub", (subscriber_t ){.deliver = deliver});

    while (true) {}
}

int main() {
    initHardwareTest();
    RegisterTask(enterBootModeTaskHardwareTest, "enterBootModeTask");
    RegisterTask(mqttTask, "mqttTask");
    StartScheduler();
}
