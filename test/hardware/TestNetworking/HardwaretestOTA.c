#define SOURCE_FILE "MAIN"
#include "Adxl345bTypedefs.h"
#include "Common.h"
#include "EnV5HwController.h"
#include "Esp.h"
#include "FlashLoader.h"
#include "FreeRtosQueueWrapper.h"
#include "FreeRtosTaskWrapper.h"
#include "MqttBroker.h"
#include "Network.h"
#include "Ota.h"
#include "Protocol.h"
#include "hardware/flash.h"
#include "hardware/structs/watchdog.h"
#include "pico/stdio_usb.h"
#include <string.h>

queue_t commandPostingsFlash;

void deliverFlashImageCommand(posting_t posting) {
    if (strlen(posting.data) == 1) {
        PRINT("RESTART");
        uint8_t flashPosition = (uint8_t)atoi(posting.data);
        restartToApplication(flashPosition);
    } else {
        freeRtosQueueWrapperPush(commandPostingsFlash, posting.data);
    }
}

_Noreturn void flashHex(void) {
    protocolSubscribeForCommand("FlashImage", (subscriber_t){.deliver = deliverFlashImageCommand});
//    protocolSubscribeForCommand("restartTo", (subscriber_t){.deliver = deliverRestartToPosition});
    
    char post[100];
    while (true) {
        if (freeRtosQueueWrapperPop(commandPostingsFlash, &post)) {
            char *token = strtok(post, ";");
            char *ip = token + 4;
            token = strtok(NULL, ";");
            char *name = token + 5;
            token = strtok(NULL, ";");
            char *size = token + 5;
            token = strtok(NULL, ";");
            char *position = token + 9;
            uint8_t flashPos = (uint8_t)atoi(position);
            PRINT("Downloading %s hex file", name);
            loadHexHTTP(ip, flashPos, name);
            PRINT("Saved %s hex file to position %i.", name, flashPos);
            
            char *apps = getStoredApplications();
            publishAliveStatusMessageWithMandatoryAttributes((status_t){.storedApplications=apps});
            free(apps);
        }
//        if (freeRtosQueueWrapperPop(commandPostingsRestart, &pos)) {
//            PRINT("%i", flashPosition);
//            restartToApplication(pos);
//        }
        freeRtosTaskWrapperTaskSleep(100);
    }
}


int main()
{
    stdio_init_all();
    espInit();
    env5HwControllerLedsInit();
    
    if(watchdog_hw->scratch[0] == FLASH_APP_UPDATED)
    {
        PRINT("pos: %lu", (watchdog_hw->scratch[1] - XIP_BASE));
        uint8_t pos = (watchdog_hw->scratch[1] - XIP_BASE) / 128 / 1024;
        PRINT("Application on position %hhu loaded!",  pos);
        watchdog_hw->scratch[0] = 0;
    } else {
        PRINT("Application NOT updated!");
    }
    
    networkTryToConnectToNetworkUntilSuccessful();
    mqttBrokerConnectToBrokerUntilSuccessful("eip://uni-due.de/es", "enV5");

    char *apps = getStoredApplications();
    publishAliveStatusMessageWithMandatoryAttributes((status_t){.storedApplications=apps});
    free(apps);

    commandPostingsFlash = freeRtosQueueWrapperCreate(5, 100);
    
    freeRtosTaskWrapperRegisterTask(flashHex, "receiveData", 5, FREERTOS_CORE_1);
    
    PRINT("STARTING SCHEDULER...");

    freeRtosTaskWrapperStartScheduler();

    return 0;
}
