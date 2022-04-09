#define SOURCE_FILE "MQTT-PUBLISH-TEST"

#include <stdio.h>

#include "TaskWrapper.h"

#include "Network.h"

#include "communicationEndpoint.h"
#include "MQTTBroker.h"

#include <string.h>
#include "malloc.h"

#include "hardwareTestHelper.h"

/***
    Connects to Wi-Fi and MQTT Broker (Change in NetworkSettings.h).
    When connected publishes each second "testData" plus incrementing integer to eip://uni-due.de/es/test.
    Can be received with the Java Integration Test IntegrationTestWhereENv5IsPublishing.
***/

void publishTestData(uint16_t i);

void _Noreturn mqttTask(void) {
    setID("eip://uni-due.de/es");
    MQTT_Broker_SetClientId("ENV5");

    for (uint16_t i = 0; i < 65536; ++i) {
        connectToNetworkAndMQTT();

        publishTestData(i);
        TaskSleep(1000);
    }
}

void publishTestData(uint16_t i) {
    char buffer[2];
    sprintf(buffer, "%d", i);
    char *data = malloc(strlen("testData") + strlen(buffer));
    strcpy(data, "testData");
    strcat(data, buffer);
    publish((Posting) {.topic="testENv5Pub", .data=data});
    free(data);
}

int main() {
    initHardwareTest();

    RegisterTask(enterBootModeTaskHardwareTest, "enterBootModeTask");
    RegisterTask(mqttTask, "mqttTask");
    StartScheduler();
}
