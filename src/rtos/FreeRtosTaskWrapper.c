#define SOURCE_FILE "TASK-WRAPPER"

#include "FreeRTOS.h"
#include "task.h"

#include "FreeRtosTaskWrapperInternal.h"
#include "eai/Common.h"
#include "eai/hal/Sleep.h"
#include "eai/rtos/FreeRtosTaskWrapper.h"

static void freeRtosTaskWrapperInternalInvokeTaskCode(void *p_taskCode) {
    TaskCodeFunc taskCode = (TaskCodeFunc)p_taskCode;
    if (taskCode) {
        taskCode();
    } else {
        PRINT("Invoking failed: taskCode not set to Function pointer");
    }
    vTaskDelete(NULL);
}

void freeRtosTaskWrapperRegisterTask(TaskCodeFunc taskCode, const char *taskName, uint8_t prio,
                                     uint8_t core) {
    TaskHandle_t handle;
    if (xTaskCreate(freeRtosTaskWrapperInternalInvokeTaskCode, taskName, 5000, (void *)taskCode,
                    prio, &handle) == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY) {
        PRINT("%s: !freeRtosTaskWrapperRegisterTask fail!: Not enough Memory available", taskName);
    } else {
        vTaskCoreAffinitySet(handle, core);
        PRINT_DEBUG("%s registered successfully.", taskName);
    }
}

void freeRtosTaskWrapperTaskSleep(int timeInMs) {
    if (xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED) {
        sleep_for_ms(timeInMs);

    } else {
        // vTaskDelay does not support delaying less than typically 10ms
        if (timeInMs < 10) {
            timeInMs = 10;
            PRINT_DEBUG("FreeRTOS does not support sleeping less than typically 10ms!");
        }
        vTaskDelay(pdMS_TO_TICKS(timeInMs));
    }
}

void freeRtosTaskWrapperStartScheduler() {
    PRINT_DEBUG("Starting scheduler");
    vTaskStartScheduler();
    PRINT_DEBUG("Creating FreeRTOS-Idle task failed because of low Memory.\n"
                "If you see this message, please make sure your device is working properly.\n"
                "exiting...");
}
