#define SOURCE_FILE "MQTT-KEEPALIVE-TEST"

/*!
 * Connects to Wi-Fi and MQTT Broker.
 * Disconnects from Broker after 5 seconds.
 * => Broker should provide LWT-Message!
 */

#include "Common.h"
#include "HardwaretestHelper.h"
#include "MqttBroker.h"
#include "Sleep.h"

int main() {
    initHardwareTest();
    connectToNetwork();
    connectToMqttBroker();

    PRINT("===== START TEST =====");

    publishAliveStatusMessageWithMandatoryAttributes(
        (status_t){.type = "enV5", .state = STATUS_STATE_ONLINE});

    sleep_for_ms(5000);

    mqttBrokerDisconnect(false);

    return EXIT_SUCCESS;
}
