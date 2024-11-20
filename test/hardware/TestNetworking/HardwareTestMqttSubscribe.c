#define SOURCE_FILE "MQTT-SUBSCRIBE-TEST"

#include "Common.h"
#include "FreeRtosQueueWrapper.h"
#include "FreeRtosTaskWrapper.h"
#include "HardwareTestHelper.h"
#include "MqttBroker.h"
#include "Protocol.h"
#include <stdio.h>

/*!
 * Connects to Wi-Fi and MQTT Broker (Change in src/configuration.h). Subscribes to topic
 * "eip://uni-due.de/es/test" and prints out the received Data. The Java Integration Test
 * IntegrationTestWhereENv5Subscribes can be used to publish the data.
 */

queue_t postings;

void deliver(posting_t posting) {
    freeRtosQueueWrapperPushFromInterrupt(postings, &posting);
}

void _Noreturn receiverTask(void) {
    PRINT("===== STARTING TEST =====");

    protocolSubscribeForData("integTestTwin", "testSub1", (subscriber_t){.deliver = deliver});
    protocolSubscribeForData("integTestTwin", "testSub2", (subscriber_t){.deliver = deliver});

    while (1) {
        posting_t post;
        if (freeRtosQueueWrapperPop(postings, &post)) {
            PRINT("Received Message: '%s' via '%s'", post.data, post.topic);
            free(post.data);
            free(post.topic);
        }
        freeRtosTaskWrapperTaskSleep(1000);
    }
}

int main() {
    initHardwareTest();
    connectToNetwork();
    connectToMqttBroker();

    publishAliveStatusMessageWithMandatoryAttributes(
        (status_t){.type = "enV5", .state = STATUS_STATE_ONLINE});

    postings = freeRtosQueueWrapperCreate(10, sizeof(posting_t));

    freeRtosTaskWrapperRegisterTask(enterBootModeTask, "watchdog_update", 0, FREERTOS_CORE_1);
    freeRtosTaskWrapperRegisterTask(receiverTask, "receive", 0, FREERTOS_CORE_0);
    freeRtosTaskWrapperStartScheduler();
}
