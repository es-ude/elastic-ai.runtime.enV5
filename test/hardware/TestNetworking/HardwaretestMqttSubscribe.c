#define SOURCE_FILE "MQTT-SUBSCRIBE-TEST"

#include "Common.h"
#include "FreeRtosTaskWrapper.h"
#include "HardwaretestHelper.h"
#include "MqttBroker.h"
#include "Protocol.h"
#include <stdio.h>

/*!
 * Connects to Wi-Fi and MQTT Broker (Change in src/configuration.h). Subscribes to topic
 * "eip://uni-due.de/es/test" and prints out the received Data. The Java Integration Test
 * IntegrationTestWhereENv5Subscribes can be used to publish the data.
 */

uint64_t arrivedMessages = 0;

void deliver(posting_t posting) {
    arrivedMessages++;
    PRINT("Received Data: %s, Message number: %llu", posting.data, arrivedMessages)
}

void _Noreturn mqttTask(void) {
    PRINT("=== STARTING TEST ===")

    connectToNetwork();
    connectToMQTT();

    protocolSubscribeForData("integTestTwin", "testSub", (subscriber_t){.deliver = deliver});

    while (1) {}
}

int main() {
    initHardwareTest();

    freeRtosTaskWrapperRegisterTask(enterBootModeTaskHardwareTest, "enterBootModeTask", 0,
                                    FREERTOS_CORE_0);
    freeRtosTaskWrapperRegisterTask(mqttTask, "mqttTask", 0, FREERTOS_CORE_0);
    freeRtosTaskWrapperStartScheduler();
}
