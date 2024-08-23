#define SOURCE_FILE "MAIN"
#include "Adxl345bTypedefs.h"
#include "Common.h"
#include "EnV5HwController.h"
#include "Esp.h"
#include "FlashLoader.h"
#include "FreeRtosQueueWrapper.h"
#include "FreeRtosTaskWrapper.h"
#include "I2c.h"
#include "MqttBroker.h"
#include "Network.h"
#include "Ota.h"
#include "Protocol.h"
#include "hardware/flash.h"
#include "hardware/i2c.h"
#include "hardware/structs/watchdog.h"
#include "pico/stdio_usb.h"
#include <string.h>

char*getAppNameOrFree(applicationFlashPosition pos) {
    const tFlashHeader* header = (const tFlashHeader*)(XIP_BASE + pos * 128 * 1024);
    if (header->isProgram== PROGRAM_IS_PRESENT)
        return (char *)header->name;
    return "FREE";
}

char message[100];
applicationFlashPosition flashPosition;

queue_t commandPostings;
queue_t commandPostings2;

void deliverFlashImageCommand(posting_t posting) {
    if (strlen(posting.data) == 1) {
        PRINT("RESTART");
        flashPosition = (applicationFlashPosition)atoi(posting.data);
        restartToApplication(flashPosition);
    } else {
        strcpy(message, posting.data);
        freeRtosQueueWrapperPush(commandPostings, message);
    }
}

_Noreturn void flashHex(void) {
    protocolSubscribeForCommand("FlashImage", (subscriber_t){.deliver = deliverFlashImageCommand});
//    protocolSubscribeForCommand("restartTo", (subscriber_t){.deliver = deliverRestartToPosition});
    
    char post[100];
    char pos[100];
    while (true) {
        if (freeRtosQueueWrapperPop(commandPostings, &post)) {
            char *token = strtok(message, ";");
            char *ip = token + 4;
            token = strtok(NULL, ";");
            char *name = token + 5;
            token = strtok(NULL, ";");
            char *size = token + 5;
            token = strtok(NULL, ";");
            char *position = token + 9;
            applicationFlashPosition flashPos = (applicationFlashPosition)atoi(position);
            PRINT("Downloading %s hex file", name);
            loadHexHTTP(ip, flashPos, name);
            PRINT("Saved %s hex file to position %i.", name, flashPos);
        }
//        freeRtosTaskWrapperTaskSleep(100);
//        if (freeRtosQueueWrapperPop(commandPostings2, &pos)) {
//            PRINT("%i", flashPosition);
//            restartToApplication(pos);
//        }
        freeRtosTaskWrapperTaskSleep(100);
//        PRINT("sleep");
    }
}

int main()
{
    stdio_init_all();
    espInit();
    env5HwControllerLedsInit();
    
    if(watchdog_hw->scratch[0] == FLASH_APP_UPDATED)
    {
        uint8_t pos = (watchdog_hw->scratch[1] - XIP_BASE) / 128 / 1024;
        PRINT("Application on position %hhu loaded!",  pos);
        watchdog_hw->scratch[0] = 0;
    } else {
        PRINT("Application NOT updated!");
    }
    
    networkTryToConnectToNetworkUntilSuccessful();
    mqttBrokerConnectToBrokerUntilSuccessful("eip://uni-due.de/es", "enV5");
    
    char *result = malloc(4 * 20 + 3);
    strcpy(result, getAppNameOrFree(FIRST));
    strcat(result, ",");
    strcat(result, getAppNameOrFree(SECOND));
    strcat(result, ",");
    strcat(result, getAppNameOrFree(THIRD));
    strcat(result, ",");
    strcat(result, getAppNameOrFree(FOURTH));
    
    publishAliveStatusMessageWithMandatoryAttributes((status_t) {.storedApps=result});
    free(result);
    
    commandPostings = freeRtosQueueWrapperCreate(5, sizeof(100));
    commandPostings2 = freeRtosQueueWrapperCreate(5, sizeof(100));
    
    freeRtosTaskWrapperRegisterTask(flashHex, "receiveData", 5, FREERTOS_CORE_1);
    
    PRINT("STARTING SCHEDULER...");

    freeRtosTaskWrapperStartScheduler();

    return 0;
}
