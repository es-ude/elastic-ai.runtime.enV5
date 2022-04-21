#define SOURCE_FILE "MQTT-STRESSTEST-PUBLISH"

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
    When connected it publishes as fast as possible to eip://uni-due.de/es/stresstest.
***/

void publishTestData(uint16_t i);

void _Noreturn mqttTask(void) {
    MQTT_Broker_setBrokerDomain("eip://uni-due.de/es/");
    MQTT_Broker_SetClientId("ENV5");

    connectToNetwork();
    connectToMQTT();

    PRINT("Start sending...")

    uint64_t i = 0;
    while (true) {
        PRINT("stress: %llu", i)
        publishTestData(i);
        i++;
    }
}

void publishTestData(uint16_t i) {
    char buffer[2];
    sprintf(buffer, "%d", i);
    char *data = malloc(strlen("testData") + strlen(buffer));
    strcpy(data, "stress: ");
    strcat(data, buffer);
    publish((Posting) {.topic="stresstest", .data=data});
    free(data);
}

int main() {
    initHardwareTest();

    RegisterTask(enterBootModeTaskHardwareTest, "enterBootModeTask");
    RegisterTask(mqttTask, "mqttTask");
    StartScheduler();
}
