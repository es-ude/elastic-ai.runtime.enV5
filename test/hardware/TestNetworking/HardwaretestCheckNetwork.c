#define SOURCE_FILE "NETWORK-CHECK-TEST"

#include "Common.h"
#include "FreeRtosTaskWrapper.h"
#include "HardwaretestHelper.h"
#include "Network.h"

/*!
 * Tries to connect to the network which is specified in NetworkConfig.c.
 * Runs connection check every 3 seconds.
 */

_Noreturn void runTest() {
    PRINT("=== STARTING TEST ===");

    while (1) {
        if (NETWORK_NO_ERROR == networkCheckConnection()) {
            PRINT("Connection ok!");
        } else {
            PRINT("Problem with connection!");
        }
        freeRtosTaskWrapperTaskSleep(3000);
    }
}

int main() {
    initHardwareTest();
    connectToNetwork();
    runTest();
}
