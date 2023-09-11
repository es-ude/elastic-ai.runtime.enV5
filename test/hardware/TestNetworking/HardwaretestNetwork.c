#define SOURCE_FILE "NETWORK-TEST"

#include "Common.h"
#include "FreeRtosTaskWrapper.h"
#include "HardwaretestHelper.h"
#include "Network.h"

/*!
 * Tries to connect to the network which is specified in NetworkSettings.h. When successful the
 * connection is closed and the process repeats.
 */

extern networkCredentials_t credentials;

_Noreturn void networkTask() {
    PRINT("=== STARTING TEST ===")

    while (1) {
        connectToNetwork();
        freeRtosTaskWrapperTaskSleep(2000);
        networkDisconnectFromNetwork();
        freeRtosTaskWrapperTaskSleep(2000);
    }
}

int main() {
    initHardwareTest();

    freeRtosTaskWrapperRegisterTask(enterBootModeTaskHardwareTest, "enterBootModeTask", 0, 0);
    freeRtosTaskWrapperRegisterTask(networkTask, "networkTask", 0, 0);
    freeRtosTaskWrapperStartScheduler();
}
