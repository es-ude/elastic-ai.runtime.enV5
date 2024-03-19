#define SOURCE_FILE "MQTT-STRESSTEST-PUBLISH"

#include "Common.h"
#include "FreeRtosQueueWrapper.h"
#include "FreeRtosTaskWrapper.h"
#include "HardwaretestHelper.h"
#include "Protocol.h"
#include <stdio.h>
#include <string.h>

/*!
 * Similar to HardwaretestMqttSubscribe.c.
 * Connects to Wi-Fi and MQTT Broker.
 * When connected, it subscribes to a topic and tries to handle messages as fast as possible.
 */

queue_t postings;

void deliver(posting_t posting) {
    freeRtosQueueWrapperPushFromInterrupt(postings, &posting);
}

_Noreturn void receiveTask(void) {
    protocolSubscribeForData("enV5", "testSub", (subscriber_t){.deliver = deliver});

    while (true) {
        posting_t post;
        if (freeRtosQueueWrapperPop(postings, &post)) {
            PRINT("Received: %s", post.data);
        }
    }
}

int main() {
    initHardwareTest();
    connectToNetwork();
    connectToMqttBroker();

    postings = freeRtosQueueWrapperCreate(20, sizeof(posting_t));

    PRINT("=== STARTING TEST ===");
    freeRtosTaskWrapperRegisterTask(enterBootModeTask, "enterBootModeTask", 0, FREERTOS_CORE_0);
    freeRtosTaskWrapperRegisterTask(receiveTask, "receive", 0, FREERTOS_CORE_1);
    freeRtosTaskWrapperStartScheduler();
}
