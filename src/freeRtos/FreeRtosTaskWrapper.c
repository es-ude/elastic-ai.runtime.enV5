#define SOURCE_FILE "TASK-WRAPPER"

#include "FreeRtosTaskWrapper.h"
#include "Common.h"
#include "FreeRtosTaskWrapperInternal.h"
#include "Sleep.h"
#include <FreeRTOS.h>
#include <task.h>

static void freeRtosTaskWrapperInternalInvokeTaskCode(void *p_taskCode) {
    TaskCodeFunc taskCode = (TaskCodeFunc)p_taskCode;
    if (taskCode)
        taskCode();
    else {
        PRINT("Invoking failed: taskCode not set to Function pointer")
    }
    vTaskDelete(NULL);
}

void freeRtosTaskWrapperRegisterTask(TaskCodeFunc taskCode, const char *taskName) {
    TaskHandle_t handle;
    if (xTaskCreate(freeRtosTaskWrapperInternalInvokeTaskCode, taskName, 10000, (void *)taskCode, 1,
                    &handle) == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY) {
        PRINT("%s: !freeRtosTaskWrapperRegisterTask fail!: Not enough Memory available", taskName)
    } else {
        PRINT("%s registered successfully.", taskName)
    }
}

void freeRtosTaskWrapperTaskSleep(int timeInMs) {
    if (xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED)
        sleep_for_ms(timeInMs);
    else {
        // vTaskDelay does not support delaying less than typically 10ms
        if (timeInMs < 10) {
            timeInMs = 10;
            PRINT("FreeRTOS does not support sleeping less than typically 10ms!")
        }
        vTaskDelay(pdMS_TO_TICKS(timeInMs));
    }
}

void freeRtosTaskWrapperStartScheduler() {
    PRINT("Starting scheduler")
    vTaskStartScheduler();
    PRINT("Creating FreeRTOS-Idle task failed because of low Memory.\nIf you "
          "see this message, please make "
          "sure your device is working properly.\nexiting...")
}
