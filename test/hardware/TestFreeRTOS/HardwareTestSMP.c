#define SOURCE_FILE "HARDWARE-TEST-FREERTOS-SMP"

#include "Common.h"
#include "EnV5HwController.h"
#include "FreeRtosTaskWrapper.h"

#include "hardware/watchdog.h"
#include "pico/bootrom.h"
#include "pico/stdlib.h"

// just here to make FreeRTOS happy
#include "FreeRTOS.h"

#include "task.h"

void initializeCommunication() {
    // check if we crash last time -> reboot into boot rom mode
    if (watchdog_enable_caused_reboot()) {
        reset_usb_boot(0, 0);
    }

    env5HwControllerInit();

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
        PRINT_DEBUG("RESET_WATCHDOG");
        watchdog_update();
        freeRtosTaskWrapperTaskSleep(2000);
    }
}

_Noreturn void task() {
    while (1) {
        PRINT("This task is running on core: %lu", vTaskCoreAffinityGet(NULL) & (1 << 0));
        freeRtosTaskWrapperTaskSleep(1000);
    }
}

int main() {
    initializeCommunication();

    freeRtosTaskWrapperRegisterTask(enterBootModeTask, "watchdog", 30, FREERTOS_CORE_0);
    freeRtosTaskWrapperRegisterTask(task, "task0", 1, FREERTOS_CORE_0);
    freeRtosTaskWrapperRegisterTask(task, "task1", 1, FREERTOS_CORE_1);
    freeRtosTaskWrapperStartScheduler();
}
