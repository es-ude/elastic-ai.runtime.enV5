#define SOURCE_FILE "MQTT-STRESSTEST-PUBLISH"

#include "hardwareTestHelper.h"
#include "TaskWrapper.h"
#include "MQTTBroker.h"
#include "Network.h"
#include "common.h"
#include "communicationEndpoint.h"
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
    MQTT_Broker_setBrokerDomain("eip://uni-due.de/es");
    MQTT_Broker_SetClientId("ENV5");

    connectToNetwork();
    connectToMQTT();

    subscribe("stresstest", (Subscriber) {.deliver=deliver});

    while (true) {
        Network_PrintIP();
        TaskSleep(1000);
    }
}

int main() {
    initHardwareTest();

    RegisterTask(enterBootModeTaskHardwareTest, "enterBootModeTask");
    RegisterTask(mqttTask, "mqttTask");
    StartScheduler();
}
