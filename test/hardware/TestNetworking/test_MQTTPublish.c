#define SOURCE_FILE "MQTT-PUBLISH-TEST"

#include "hardwareTestHelper.h"
#include "TaskWrapper.h"
#include "Network.h"
#include "MQTTBroker.h"
#include "protocol.h"
#include <stdio.h>
#include <string.h>
#include <malloc.h>

/***
    Connects to Wi-Fi and MQTT Broker (Change in NetworkSettings.h).
    When connected publishes each second "testData" plus incrementing integer to eip://uni-due.de/es/DATA/test.
    Can be received with the Java Integration Test IntegrationTestWhereENv5IsPublishing.
***/

void publishTestData(uint16_t i) {
    char buffer[2];
    sprintf(buffer, "%d", i);
    char *data = malloc(strlen("testData") + strlen(buffer));
    strcpy(data, "testData");
    strcat(data, buffer);
    publishData("testENv5Pub", data);
    free(data);
}

void _Noreturn mqttTask(void) {
    MQTT_Broker_setBrokerDomain("eip://uni-due.de/es");
    MQTT_Broker_SetClientId("ENV5");

    uint64_t i = 0;
    while (true) {
        connectToNetwork();
        connectToMQTT();

        publishTestData(i);
        i++;
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
