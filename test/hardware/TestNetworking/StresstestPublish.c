#define SOURCE_FILE "MQTT-STRESSTEST-PUBLISH"

#include "Common.h"
#include "FreeRtosTaskWrapper.h"
#include "HardwaretestHelper.h"
#include "Protocol.h"

#include <stdio.h>

/*!
 * Similar to HardwaretestMqttPublish.c.
 * Connects to Wi-Fi and MQTT Broker.
 * When connected, it publishes as fast as possible!
 */

void _Noreturn sendTask(void) {
    uint16_t messageCounter = 0;
    while (true) {
        char data[17];
        snprintf(data, 17, "testData - %i", messageCounter);
        PRINT("Send Message '%s'", data);
        protocolPublishData("stresstestPub", data);
        messageCounter++;
    }
}

int main() {
    initHardwareTest();
    connectToNetwork();
    connectToMqttBroker();

    PRINT("=== STARTING TEST ===");
    freeRtosTaskWrapperRegisterTask(enterBootModeTask, "enterBootModeTask", 0, FREERTOS_CORE_0);
    freeRtosTaskWrapperRegisterTask(sendTask, "send", 0, FREERTOS_CORE_1);
    freeRtosTaskWrapperStartScheduler();
}
