#define SOURCE_FILE "MQTT-SUB_UNSUB-TEST"

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
 * Connects to Wi-Fi and MQTT Broker (Change in src/configuration.h).
 * Subscribes and publishes to topic and prints the received Data.
 */

queue_t postings;
char dataTopic[] = "testPubSub";

void deliver(posting_t posting) {
    freeRtosQueueWrapperPushFromInterrupt(postings, &posting);
}

_Noreturn void subscriptionTask(void) {
    while (1) {
        protocolSubscribeForData("enV5", dataTopic, (subscriber_t){.deliver = deliver});
        PRINT("Subscribed!");
        freeRtosTaskWrapperTaskSleep(3000);

        protocolUnsubscribeFromData("enV5", dataTopic, (subscriber_t){.deliver = deliver});
        PRINT("Unsubscribed!");
        freeRtosTaskWrapperTaskSleep(3000);
    }
}

_Noreturn void receiverTask(void) {
    while (1) {
        posting_t post;
        if (freeRtosQueueWrapperPop(postings, &post)) {
            PRINT("Received Message: '%s' via '%s'", post.data, post.topic);
        }
        freeRtosTaskWrapperTaskSleep(100);
    }
}

_Noreturn void senderTask(void) {
    uint16_t messageCounter = 0;
    while (1) {
        freeRtosTaskWrapperTaskSleep(700);
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

    postings = freeRtosQueueWrapperCreate(10, sizeof(posting_t));

    PRINT("=== STARTING TEST ===");
    freeRtosTaskWrapperRegisterTask(enterBootModeTask, "enterBootModeTask", 0, FREERTOS_CORE_0);
    freeRtosTaskWrapperRegisterTask(senderTask, "send", 0, FREERTOS_CORE_1);
    freeRtosTaskWrapperRegisterTask(subscriptionTask, "subscription", 0, FREERTOS_CORE_0);
    freeRtosTaskWrapperRegisterTask(receiverTask, "receive", 0, FREERTOS_CORE_1);
    freeRtosTaskWrapperStartScheduler();
}
