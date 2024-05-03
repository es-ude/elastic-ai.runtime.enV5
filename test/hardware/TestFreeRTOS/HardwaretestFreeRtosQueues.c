/*!
 * This test aims to provide a test for FreeRTOS queues.
 * It tests the exchange of a message content after pushing it to the queue,
 * by replacing the pointed storage.
 * It also tests the behavior in case of an overflowing queue,
 * because the sleep inside the receiver task is longer than the send task.
 */

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

_Noreturn void enterBootModeTask() {
    while (1) {
        PRINT_DEBUG("RESET WATCHDOG");
        watchdog_update();
        freeRtosTaskWrapperTaskSleep(2000);
    }
}

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
            PRINT_DEBUG("Added message to queue.");
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

int main(void) {
    initHardware();

    PRINT("===== CREATE QUEUE =====");
    messageQueue = freeRtosQueueWrapperCreate(5, sizeof(message_t));
    if (NULL == messageQueue) {
        PRINT("Creating Queue failed!");
        reset_usb_boot(0, 0);
    }

    freeRtosTaskWrapperRegisterTask(senderTask, "send", 0, FREERTOS_CORE_0);
    freeRtosTaskWrapperRegisterTask(receiverTask, "receive", 0, FREERTOS_CORE_1);
    freeRtosTaskWrapperRegisterTask(enterBootModeTask, "watchdog_update", 1, FREERTOS_CORE_0);
    freeRtosTaskWrapperStartScheduler();
}
