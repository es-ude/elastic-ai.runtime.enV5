#define SOURCE_FILE "MQTT-SUBSCRIBE-TEST"

#include <stdio.h>

#include "TaskWrapper.h"
#include "QueueWrapper.h"

#include "communicationEndpoint.h"
#include "MQTTBroker.h"

#include "common.h"

#include "hardwareTestHelper.h"

/***
    Connects to Wi-Fi and MQTT Broker (Change in NetworkSettings.h).
    Subscribes to topic "eip://uni-due.de/es/test" and prints out the received Data.
    The Java Integration Test IntegrationTestWhereENv5Subscribes can be used to publish the data.
***/

void deliver(Posting posting) {
    PRINT("Received Data: %s", posting.data)
}

void _Noreturn mqttTask(void) {
    MQTT_Broker_setBrokerDomain("eip://uni-due.de/es");
    MQTT_Broker_SetClientId("ENV5");

    connectToNetwork();
    connectToMQTT();

    subscribe("testENv5Sub", (Subscriber) {.deliver=deliver});

    while (true) {
        connectToNetwork();
        connectToMQTT();
        TaskSleep(1000);
    }
    MQTT_Broker_freeBrokerDomain();
}

int main() {
    initHardwareTest();

    RegisterTask(enterBootModeTaskHardwareTest, "enterBootModeTask");
    RegisterTask(mqttTask, "mqttTask");
    StartScheduler();
}
