#define SOURCE_FILE "NETWORK-CHECK-TEST"

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
    connectToNetwork();

    PRINT("=== STARTING TEST ===")

    while (1) {
        networkcheckConnection();
        freeRtosTaskWrapperTaskSleep(3000);
    }
}

int main() {
    initHardwareTest();

    freeRtosTaskWrapperRegisterTask(enterBootModeTaskHardwareTest, "enterBootModeTask", 0);
    freeRtosTaskWrapperRegisterTask(networkTask, "networkTask", 0);
    freeRtosTaskWrapperStartScheduler();
}
