#define SOURCE_FILE "MQTT-STRESSTEST-PUBLISH/SUBSCRIBE"

#include "Common.h"
#include "FreeRtosQueueWrapper.h"
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
        freeRtosTaskWrapperTaskSleep(1);
    }
}

void _Noreturn sendTask(void) {
    uint16_t messageCounter = 0;
    while (true) {
        char data[17];
        snprintf(data, 17, "testData - %i", messageCounter);
        PRINT("Send Message '%s'", data);
        protocolPublishData("stresstestPub", data);
        messageCounter++;
        freeRtosTaskWrapperTaskSleep(1);
    }
}

int main() {
    initHardwareTest();

    freeRtosTaskWrapperRegisterTask(enterBootModeTask, "enterBootModeTask", 0, FREERTOS_CORE_0);
    freeRtosTaskWrapperRegisterTask(receiveTask, "receive", 0, FREERTOS_CORE_1);
    freeRtosTaskWrapperRegisterTask(sendTask, "sendTask", 0, FREERTOS_CORE_1);
    freeRtosTaskWrapperStartScheduler();
}
