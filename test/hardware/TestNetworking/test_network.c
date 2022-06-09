#define SOURCE_FILE "NETWORK-TEST"

#include "hardwareTestHelper.h"
#include "TaskWrapper.h"
#include "Network.h"
#include "common.h"

/***
    Tries to connect to the network which is specified in NetworkSettings.h. When successful the connection is closed
    and the process repeats.
***/

extern NetworkCredentials credentials;

_Noreturn void networkTask() {
    PRINT("Starting Test")
    while (true) {
        connectToNetwork();
        TaskSleep(1000);
        while (NetworkStatus.WIFIStatus == CONNECTED) {
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
