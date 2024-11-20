#define SOURCE_FILE "NETWORK-TEST"

#include "Common.h"
#include "FreeRtosTaskWrapper.h"
#include "HardwareTestHelper.h"
#include "Network.h"

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
