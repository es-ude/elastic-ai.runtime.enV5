#define SOURCE_FILE "MQTT Subscribe Test"

#include <stdio.h>

#include "TaskWrapper.h"
#include "QueueWrapper.h"

#include "communicationEndpoint.h"
#include "espBroker.h"

#include "common.h"

#include "hardwareTestHelper.h"

/***
    Connects to Wi-Fi and MQTT Broker (Change in NetworkSettings.h).
    Subscribes to topic "eip://uni-due.de/es/test" and prints out the received Data.
    The Java Integration Test IntegrationTestWhereENv5Subscribes can be used to publish the data.
***/

void deliver(Posting posting) {
    PRINT("RECEIVED: %s", posting.data)
}

void _Noreturn mqttTask(void) {
    setID("eip://uni-due.de/es");
    ESP_MQTT_SetClientId("ENV5");

    connectToNetworkAndMQTT();

    subscribe("test", (Subscriber) {.deliver=deliver});

    while (true) {
        connectToNetworkAndMQTT();
        TaskSleep(1000);
    }
}

int main() {
    initHardwareTest();

    RegisterTask(enterBootModeTaskHardwareTest, "enterBootModeTask");
    RegisterTask(mqttTask, "mqttTask");
    StartScheduler();
}
