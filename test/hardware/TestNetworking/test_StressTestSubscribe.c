#define SOURCE_FILE "MQTT-STRESSTEST-PUBLISH"

#include "hardwareTestHelper.h"
#include "TaskWrapper.h"
#include "MQTTBroker.h"
#include "common.h"
#include "protocol.h"
#include <stdio.h>
#include <string.h>

/***
    Similar to test_MQTTSubscribe.c
    Connects to Wi-Fi and MQTT Broker (Change in NetworkSettings.h).
    When connected it subscribes data published to eip://uni-due.de/es/stresstest.
***/

void deliver(Posting posting) {
    PRINT("Received Data: \"%s\", String length: \"%d\"", posting.data, strlen(posting.data))
}

_Noreturn void mqttTask(void) {
    connectToNetwork();
    connectToMQTT();

    subscribeForData("stresstestSub", (Subscriber) {.deliver=deliver});

    while (true) {
        TaskSleep(1000);
    }
}

int main() {
    initHardwareTest();
    RegisterTask(mqttTask, "mqttTask");
    StartScheduler();
}
