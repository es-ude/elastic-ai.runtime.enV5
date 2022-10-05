#define SOURCE_FILE "NETWORK-TEST"

#include "Network.h"
#include "TaskWrapper.h"
#include "common.h"
#include "hardwareTestHelper.h"

/*!
 * Tries to connect to the network which is specified in NetworkSettings.h. When successful the
 * connection is closed and the process repeats.
 */

extern NetworkCredentials_t credentials;

_Noreturn void networkTask() {
    PRINT("=== STARTING TEST ===")

    while (1) {
        connectToNetwork();
        TaskSleep(2000);
        network_DisconnectFromNetwork();
        TaskSleep(2000);
    }
}

int main() {
    initHardwareTest();
    
    RegisterTask(enterBootModeTaskHardwareTest, "enterBootModeTask");
    RegisterTask(networkTask, "networkTask");
    StartScheduler();
}
