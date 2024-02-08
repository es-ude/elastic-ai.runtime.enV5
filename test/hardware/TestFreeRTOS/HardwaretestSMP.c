#define SOURCE_FILE "SMP"

#include "Common.h"
#include "FreeRTOS.h"
#include "FreeRtosQueueWrapper.h"
#include "FreeRtosTaskWrapper.h"
#include "pico/stdio.h"
#include "pico/stdio_usb.h"
#include "task.h"

_Noreturn void task() {
    for (;;) {
        PRINT("This task is running on core: %lu", vTaskCoreAffinityGet(NULL) & (1 << 0));
        freeRtosTaskWrapperTaskSleep(1000);
    }
}

int main() {
    stdio_init_all();
    // waits for usb connection, REMOVE to continue without waiting for connection
    while ((!stdio_usb_connected())) {}
    PRINT("");
    // create FreeRTOS task queue
    freeRtosQueueWrapperCreate();

    freeRtosTaskWrapperRegisterTask(task, "task0", 0, FREERTOS_CORE_0);
    freeRtosTaskWrapperRegisterTask(task, "task1", 0, FREERTOS_CORE_1);
    freeRtosTaskWrapperStartScheduler();
}
