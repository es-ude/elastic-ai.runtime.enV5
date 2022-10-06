#define SOURCE_FILE "NETWORK-TEST"

#include "Common.h"
#include "FreeRtosTaskWrapper.h"
#include "HardwaretestHelper.h"
#include "Network.h"

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
