#define SOURCE_FILE "SMP"

#include "Common.h"
#include "FreeRtosQueueWrapper.h"
#include "FreeRtosTaskWrapper.h"
#include "pico/stdio.h"
#include "pico/stdio_usb.h"

void task0() {

    for(;;) {
        PRINT("0")
//        freeRtosTaskWrapperTaskSleep(100);
    }
}

void task1() {
    for (;;) {
        PRINT("1")
//        freeRtosTaskWrapperTaskSleep(100);
    }

}

int main() {
    stdio_init_all();
    // waits for usb connection, REMOVE to continue without waiting for connection
    while ((!stdio_usb_connected())) {}
    PRINT("")
    // create FreeRTOS task queue
    // create FreeRTOS task queue
    freeRtosQueueWrapperCreate();

    freeRtosTaskWrapperRegisterTask(task0, "task0", 0, 0);
    freeRtosTaskWrapperRegisterTask(task1, "task1", 0, 0);
    freeRtosTaskWrapperStartScheduler();
}


