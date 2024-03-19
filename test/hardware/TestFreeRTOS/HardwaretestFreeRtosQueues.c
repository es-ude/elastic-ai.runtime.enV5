#define SOURCE_FILE "HARDWARE-TEST-FREERTOS-QUEUES"

#include "Common.h"
#include "FreeRtosQueueWrapper.h"
#include "FreeRtosTaskWrapper.h"
#include "enV5HwController.h"

#include "hardware/watchdog.h"
#include "pico/bootrom.h"
#include "pico/stdio_usb.h"

#include "stdlib.h"
#include "string.h"

queue_t messageQueue;

char staticMessage[] = "STATIC MSG";
char dynamicMessage[] = "DYNAMIC MSG";

typedef struct message {
    uint16_t messageId;
    char *content;
} message_t;

_Noreturn void senderTask() {
    uint16_t messageId = 0;
    while (1) {
        message_t message = {.messageId = messageId, .content = calloc(1, 15)};
        strcpy(message.content, staticMessage);
        if (freeRtosQueueWrapperPush(messageQueue, &message)) {
            // Modify all messages with odd ID to demonstrate dynamic messages
            if (messageId % 2) {
                strcpy(message.content, dynamicMessage);
            }
            messageId++;
        } else {
            PRINT("Failed to add message to queue.");
        }

        freeRtosTaskWrapperTaskSleep(2000);
    }
}

_Noreturn void receiverTask() {
    while (1) {
        message_t message;
        if (freeRtosQueueWrapperPop(messageQueue, &message)) {
            PRINT("Message with ID '%i' was: '%s'", message.messageId, message.content);
            free(message.content);
        } else {
            PRINT("No message available!");
        }

        freeRtosTaskWrapperTaskSleep(3500);
    }
}

void initHardware() {
    // check if we crash last time -> reboot into boot rom mode
    if (watchdog_enable_caused_reboot()) {
        reset_usb_boot(0, 0);
    }

    env5HwInit();

    // initialize the serial output
    stdio_init_all();
    while ((!stdio_usb_connected())) {
        // wait for serial connection
    }

    // enables watchdog timer (5s)
    watchdog_enable(5000, 1);
}

int main(void) {
    initHardware();

    PRINT("===== CREATE QUEUE =====");
    messageQueue = freeRtosQueueWrapperCreate(5, sizeof(message_t));
    if (NULL == messageQueue) {
        PRINT("Creating Queue failed!");
        reset_usb_boot(0, 0);
    }

    PRINT("===== START TEST =====");
    freeRtosTaskWrapperRegisterTask(senderTask, "sendTask", 0, FREERTOS_CORE_0);
    freeRtosTaskWrapperRegisterTask(receiverTask, "sendTask", 0, FREERTOS_CORE_1);
    freeRtosTaskWrapperStartScheduler();
}
