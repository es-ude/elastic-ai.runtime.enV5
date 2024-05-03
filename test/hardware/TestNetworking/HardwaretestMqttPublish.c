#define SOURCE_FILE "MQTT-PUBLISH-TEST"

#include "Common.h"
#include "FreeRtosTaskWrapper.h"
#include "HardwaretestHelper.h"
#include "MqttBroker.h"
#include "Protocol.h"

/*!
 * Connects to Wi-Fi and MQTT Broker.
 * When connected publishes each second "testData" plus incrementing integer to
 * eip://uni-due.de/es/DATA/test.
 * Can be received with the Java Integration TestIntegrationTestWhereENv5IsPublishing.
 */

char pubTopic[] = "testPub";

void _Noreturn runTest(void) {
    PRINT("===== STARTING TEST =====");

    uint16_t messageCounter = 0;
    while (1) {
        char data[17];
        snprintf(data, 17, "testData - %i", messageCounter);
        PRINT("Message: '%s'", data);
        protocolPublishData(pubTopic, data);

        messageCounter++;

        freeRtosTaskWrapperTaskSleep(1000);
    }
}

int main() {
    initHardwareTest();
    connectToNetwork();
    connectToMqttBroker();
    publishAliveStatusMessageWithMandatoryAttributes(
        (status_t){.type = "enV5", .state = STATUS_STATE_ONLINE, .data = pubTopic});
    runTest();
}
