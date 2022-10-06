#define SOURCE_FILE "FreeRTOS-QUEUE-TEST"

#include "Common.h"
#include "FreeRtosQueueWrapper.h"
#include "FreeRtosTaskWrapper.h"
#include <hardware/watchdog.h>
#include <pico/bootrom.h>
#include <pico/stdio_usb.h>
#include <pico/time.h>
#include <stdlib.h>

_Noreturn void SenderTask() {
    QueueMessage message;
    int messageCounter = 0;
    char *data = malloc(10);
    message.Data = data;

    while (1) {
        sprintf(data, "%i", messageCounter);
        messageCounter++;

        if (QueueSend(message)) {
            PRINT("Send message `%s` into Queue", data)
        } else {
            PRINT("Failed to send message into queue")
        }

        TaskSleep(250);
    }
}

_Noreturn void ReceiverTask() {
    QueueMessage message;

    while (1) {
        if (!QueueReceive(&message)) {
            PRINT("Something went Wrong!\n")
        } else {
            PRINT("Received: `%s`", message.Data)
        }

        // enter boot mode if selected
        if (getchar_timeout_us(10) == 'r' || !stdio_usb_connected()) {
            reset_usb_boot(0, 0);
        }

        // reset watchdog timer
        watchdog_update();

        TaskSleep(250);
    }
}

void initHardware() {
    // init usb and wait for user connection
    stdio_init_all();
    while ((!stdio_usb_connected())) {}
    TaskSleep(1000);

    // start watchdog timer
    watchdog_enable(2000, 1);

    // create queue
    CreateQueue();
}

void testDynamicMessageChanges() {
    // create message
    char data[10];
    snprintf(data, 6, "hallo");
    QueueMessage message;
    message.Data = data;

    // send message into queue
    QueueSend(message);

    // modify message data after send
    message.Data[0] = 'a';

    // receive modified message
    QueueReceive(&message);
    PRINT("Expected: `aallo`; Received: `%s`", message.Data)
}

int main(void) {
    // Did we crash last time? -> reboot into boot mode
    if (watchdog_enable_caused_reboot()) {
        reset_usb_boot(0, 0);
    }
    initHardware();

    PRINT("===== START TEST: dynamic vs. static messages =====")
    testDynamicMessageChanges();

    // test async
    PRINT("===== START TEST: asynchronous messages =====")
    RegisterTask(SenderTask, "sendMessage");
    RegisterTask(ReceiverTask, "receiveTask");
    StartScheduler();
}
