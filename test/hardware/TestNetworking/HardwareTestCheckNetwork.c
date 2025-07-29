#define SOURCE_FILE "NETWORK-CHECK-TEST"

#include "HardwareTestHelper.h"
#include "eai/Common.h"
#include "eai/network/Network.h"
#include "eai/rtos/FreeRtosTaskWrapper.h"

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
