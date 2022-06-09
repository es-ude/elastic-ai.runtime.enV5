#define SOURCE_FILE "MQTT-SUBSCRIBE-TEST"

#include "hardwareTestHelper.h"
#include "TaskWrapper.h"
#include "MQTTBroker.h"
#include "common.h"
#include "protocol.h"
#include <stdio.h>

/***
    Connects to Wi-Fi and MQTT Broker (Change in NetworkSettings.h).
    Subscribes to topic "eip://uni-due.de/es/test" and prints out the received Data.
    The Java Integration Test IntegrationTestWhereENv5Subscribes can be used to publish the data.
***/

uint64_t arrivedMessages = 0;

void deliver(Posting posting) {
    arrivedMessages++;
    PRINT("Received Data: %s, Message number: %llu", posting.data, arrivedMessages)
}

void _Noreturn mqttTask(void) {
    connectToNetwork();
    connectToMQTT();

    subscribeForData("testSub", (Subscriber) {.deliver=deliver});

    while (true) {
        connectToNetwork();
        connectToMQTT();
        TaskSleep(1000);
    }
}

int main() {
    initHardwareTest();

    RegisterTask(enterBootModeTaskHardwareTest, "enterBootModeTask");
    RegisterTask(mqttTask, "mqttTask");
    StartScheduler();
}
