#define SOURCE_FILE "MQTT-PUBLISH-TEST"

#include "hardwareTestHelper.h"
#include "TaskWrapper.h"
#include "Network.h"
#include "MQTTBroker.h"

#include <stdlib.h>
#include <communicationEndpoint.h>
#include <stdio.h>
#include <string.h>

/***
    Connects to Wi-Fi and MQTT Broker (Change in NetworkSettings.h).
    When connected publishes each second "testData" plus incrementing integer to eip://uni-due.de/es/test.
    Can be received with the Java Integration Test IntegrationTestWhereENv5IsPublishing.
***/

void publishTestData(uint16_t i) {
    char buffer[2];
    sprintf(buffer, "%d", i);
    char *data = malloc(strlen("testData") + strlen(buffer));
    strcpy(data, "testData");
    strcat(data, buffer);
    publish((Posting) {.topic="testENv5Pub", .data=data});
    free(data);
}

void _Noreturn mqttTask(void) {
    MQTT_Broker_setBrokerDomain("eip://uni-due.de/es");
    MQTT_Broker_SetClientId("ENV5");

    for (uint16_t i = 0; i < 65536; ++i) {
        connectToNetwork();
        connectToMQTT();

        publishTestData(i);
        TaskSleep(1000);
    }
}

int main() {
    initHardwareTest();

    RegisterTask(enterBootModeTaskHardwareTest, "enterBootModeTask");
//    RegisterTask(mqttTask, "mqttTask");
    StartScheduler();
}
