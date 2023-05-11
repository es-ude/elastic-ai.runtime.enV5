#define SOURCE_FILE "MQTT-PUBLISH-RUNTIME"

#include "Common.h"
#include "FreeRtosTaskWrapper.h"
#include "HardwaretestHelper.h"
#include "MqttBroker.h"
#include "Protocol.h"
#include <malloc.h>
#include <stdio.h>
#include <string.h>

/*!
 * Connects to Wi-Fi and MQTT Broker (Change in src/configuration.h). When connected publishes each
 * second "testData" plus incrementing integer to eip://uni-due.de/es/DATA/test. Can be received
 * with the Java Integration TestIntegrationTestWhereENv5IsPublishing.
 */

void publishTestDataSram(uint16_t i) {
    char buffer[6];
    sprintf(buffer, "%i.100", i);
    char *data = malloc(strlen(buffer) + 1);
    strcpy(data, buffer);
    protocolPublishData("sram", data);
    free(data);
}

void publishTestDataWifi(uint16_t i) {
    char buffer[6];
    sprintf(buffer, "%i.200", i);
    char *data = malloc(strlen(buffer) + 1);
    strcpy(data, buffer);
    protocolPublishData("wifi", data);
    free(data);
}

void _Noreturn mqttTask(void) {
    PRINT("=== STARTING TEST ===")

    connectToNetwork();
    connectToMQTT();

    uint64_t messageData = 0;
    while (1) {
        publishTestDataSram(messageData);
        publishTestDataWifi(messageData);
        messageData++;
        freeRtosTaskWrapperTaskSleep(1000);
    }
}

int main() {
    initHardwareTest();

    freeRtosTaskWrapperRegisterTask(enterBootModeTaskHardwareTest, "enterBootModeTask");
    freeRtosTaskWrapperRegisterTask(mqttTask, "mqttTask");
    freeRtosTaskWrapperStartScheduler();
}
