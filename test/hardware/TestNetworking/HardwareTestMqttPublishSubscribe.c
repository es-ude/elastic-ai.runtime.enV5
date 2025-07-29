#define SOURCE_FILE "MQTT-PUBSUB-TEST"

#include <stdio.h>

#include "HardwareTestHelper.h"
#include "Protocol.h"
#include "eai/Common.h"
#include "eai/rtos/FreeRtosQueueWrapper.h"
#include "eai/rtos/FreeRtosTaskWrapper.h"

/*!
 * Connects to Wi-Fi and MQTT Broker.
 * Subscribes and publishes to topic "eip://uni-due.de/es/test" and prints the received Data.
 */

queue_t postings;
char dataTopic[] = "testPubSub";

void deliver(posting_t posting) {
    freeRtosQueueWrapperPushFromInterrupt(postings, &posting);
}

_Noreturn void receiverTask(void) {
    protocolSubscribeForData("enV5", dataTopic, (subscriber_t){.deliver = deliver});

    while (1) {
        posting_t post;
        if (freeRtosQueueWrapperPop(postings, &post)) {
            PRINT("Received Message: '%s' via '%s'", post.data, post.topic);
        }
        freeRtosTaskWrapperTaskSleep(1000);
    }
}

_Noreturn void senderTask(void) {
    uint16_t messageCounter = 0;
    while (1) {
        freeRtosTaskWrapperTaskSleep(1000);
        char data[17];
        snprintf(data, 17, "testData - %i", messageCounter);
        PRINT("Send Message: '%s' via '%s'", data, dataTopic);
        protocolPublishData(dataTopic, data);
        messageCounter++;
    }
}

int main() {
    initHardwareTest();
    connectToNetwork();
    connectToMqttBroker();

    postings = freeRtosQueueWrapperCreate(5, sizeof(posting_t));

    PRINT("===== STARTING TEST =====");
    freeRtosTaskWrapperRegisterTask(enterBootModeTask, "enterBootModeTask", 0, FREERTOS_CORE_0);
    freeRtosTaskWrapperRegisterTask(receiverTask, "receive", 0, FREERTOS_CORE_1);
    freeRtosTaskWrapperRegisterTask(senderTask, "send", 0, FREERTOS_CORE_1);
    freeRtosTaskWrapperStartScheduler();
}
