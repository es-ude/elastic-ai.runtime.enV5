#define SOURCE_FILE "MQTT-KEEPALIVE-TEST"

#include "Common.h"
#include "FreeRtosTaskWrapper.h"
#include "HardwaretestHelper.h"
#include "MqttBroker.h"

void _Noreturn mqttTask(void) {
    PRINT("=== STARTING TEST ===")

    connectToNetwork();
    connectToMQTT();

    while (true) {}
}

int main() {
    initHardwareTest();
    freeRtosTaskWrapperRegisterTask(enterBootModeTaskHardwareTest, "enterBootModeTask", 0, 0);
    freeRtosTaskWrapperRegisterTask(mqttTask, "mqttTask", 0, 0);
    freeRtosTaskWrapperStartScheduler();
}
