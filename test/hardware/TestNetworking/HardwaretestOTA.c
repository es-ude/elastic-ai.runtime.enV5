#define SOURCE_FILE "MAIN"
#include "Common.h"
#include "EnV5HwController.h"
#include "Esp.h"
#include "FlashLoader.h"
#include "FreeRtosTaskWrapper.h"
#include "MqttBroker.h"
#include "Network.h"
#include "Ota.h"
#include "hardware/flash.h"
#include "hardware/structs/watchdog.h"
#include "pico/stdio_usb.h"

int main()
{
    stdio_init_all();
    espInit();
    env5HwControllerLedsInit();

    networkTryToConnectToNetworkUntilSuccessful();
    mqttBrokerConnectToBrokerUntilSuccessful("eip://uni-due.de/es", "enV5");

    PRINT("scratch: %lu", watchdog_hw->scratch[0]);
    
    if(watchdog_hw->scratch[0] == FLASH_APP_UPDATED)
    {
        PRINT("Application just updated!");
        watchdog_hw->scratch[0] = 0;
    } else {
        PRINT("Application not updated!");
    }

    freeRtosTaskWrapperRegisterTask(flashHex, "receiveData", 5, FREERTOS_CORE_1);

    PRINT("STARTING SCHEDULER...");

    freeRtosTaskWrapperStartScheduler();

    return 0;
}
