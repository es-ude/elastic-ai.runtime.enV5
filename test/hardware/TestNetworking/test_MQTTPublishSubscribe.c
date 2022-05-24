#define SOURCE_FILE "MQTT-PUBLISH/SUBSCRIBE-TEST"

#include <stdio.h>

#include "TaskWrapper.h"

#include "Network.h"

#include "communicationEndpoint.h"
#include "MQTTBroker.h"

#include <string.h>
#include "malloc.h"
#include "common.h"

#include "hardwareTestHelper.h"

/***
    Connects to Wi-Fi and MQTT Broker (Change in NetworkSettings.h).
    Subscribes and publishes to topic "eip://uni-due.de/es/test" and prints the received Data.
***/

void publishTestData(uint16_t i) {
    char buffer[2];
    sprintf(buffer, "%d", i);
    char *data = malloc(strlen("testData") + strlen(buffer));
    strcpy(data, "testData");
    strcat(data, buffer);
    publish((Posting) {.topic="testENv5PubSub", .data=data});
    free(data);
}

void deliver(Posting posting) {
    PRINT("Received Data: %s", posting.data)
}

void _Noreturn mqttTask(void) {
    MQTT_Broker_setBrokerDomain("eip://uni-due.de/es");
    MQTT_Broker_SetClientId("ENV5");

    connectToNetwork();
    connectToMQTT();

    subscribe("testENv5PubSub", (Subscriber) {.deliver=deliver});

    for (uint16_t i = 0; i < 65536; ++i) {
        connectToNetwork();
        connectToMQTT();
        publishTestData(i);
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
