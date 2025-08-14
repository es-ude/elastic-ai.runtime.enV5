#define SOURCE_FILE "NETWORK-TEST"

#include "HardwareTestHelper.h"
#include "eai/Common.h"
#include "eai/network/Network.h"
#include "eai/rtos/FreeRtosTaskWrapper.h"

/*!
 * Tries to connect to the Wi-Fi.
 * When successful, the connection is closed and the process repeats.
 */

_Noreturn void runTest() {
    PRINT("=== STARTING TEST ===");

    while (1) {
        networkTryToConnectToNetworkUntilSuccessful();
        PRINT("CONNECTED!");
        freeRtosTaskWrapperTaskSleep(2000);
        networkDisconnectFromNetwork();
        PRINT("DISCONNECTED!");
        freeRtosTaskWrapperTaskSleep(2000);
    }
}

int main() {
    initHardwareTest();
    runTest();
}
