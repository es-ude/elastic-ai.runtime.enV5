#define SOURCE_FILE "MQTT-STRESSTEST-PUBLISH"

#include <stdio.h>

#include "TaskWrapper.h"

#include "communicationEndpoint.h"
#include "MQTTBroker.h"

#include "common.h"

#include "hardwareTestHelper.h"

/***
    Similar to test_MQTTSubscribe.c
    Connects to Wi-Fi and MQTT Broker (Change in NetworkSettings.h).
    When connected it subscribes data published to eip://uni-due.de/es/stresstest.
***/

void deliver(Posting posting) {
    PRINT("Received Data: %s", posting.data)
}

void mqttTask(void) {
    MQTT_Broker_setBrokerDomain("eip://uni-due.de/es");
    MQTT_Broker_SetClientId("ENV5");

    connectToNetwork();
    connectToMQTT();

    subscribe("stresstest", (Subscriber) {.deliver=deliver});
}

int main() {
    initHardwareTest();

    RegisterTask(enterBootModeTaskHardwareTest, "enterBootModeTask");
    RegisterTask(mqttTask, "mqttTask");
    StartScheduler();
}
