#define SOURCE_FILE "NETWORK-TEST"

#include "Network.h"
#include "TaskWrapper.h"
#include "common.h"
#include "esp.h"
#include "hardwareTestHelper.h"

/***
    Tries to connect to the network which is specified in NetworkSettings.h.
When successful the connection is closed and the process repeats.
***/

extern NetworkCredentials_t credentials;

_Noreturn void networkTask() {
    PRINT("=== STARTING TEST ===")

    while (true) {
        connectToNetwork();
        TaskSleep(1000);
        while (ESP_Status.WIFIStatus == CONNECTED) {
            Network_DisconnectFromNetwork();
        }
        TaskSleep(1000);
    }
}

int main() {
    initHardwareTest();
    RegisterTask(enterBootModeTaskHardwareTest, "enterBootModeTask");
    RegisterTask(networkTask, "networkTask");
    StartScheduler();
}
