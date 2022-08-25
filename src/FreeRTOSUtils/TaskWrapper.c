#define SOURCE_FILE "TASK-WRAPPER"

#include "TaskWrapper.h"
#include "FreeRTOS.h"
#include "common.h"
#include "pico/time.h"
#include "task.h"

static void InvokeTaskCode(void *p_taskCode) {
    TaskCodeFunc taskCode = (TaskCodeFunc)p_taskCode;
    if (taskCode)
        taskCode();
    else {
        PRINT("Invoking failed: taskCode not set to Function pointer")
    }
    vTaskDelete(NULL);
}

void RegisterTask(TaskCodeFunc p_taskCode, const char *const p_taskName) {
    TaskHandle_t handle;
    if (xTaskCreate(InvokeTaskCode, p_taskName, 10000, (void *)p_taskCode, 1, &handle) ==
        errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY) {
        PRINT("%s: !RegisterTask fail!: Not enough Memory available", p_taskName)
    } else {
        PRINT("%s registered successfully.", p_taskName)
    }
}

void TaskSleep(int timeInMs) {
    if (xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED)
        sleep_ms(timeInMs);
    else {
        // vTaskDelay does not support delaying less than typically 10ms
        if (timeInMs < 10) {
            timeInMs = 10;
            PRINT("FreeRTOS does not support sleeping less than typically 10ms!")
        }
        vTaskDelay(pdMS_TO_TICKS(timeInMs));
    }
}

void StartScheduler() {
    PRINT("Starting scheduler")
    vTaskStartScheduler();
    PRINT("Creating FreeRTOS-Idle task failed because of low Memory.\nIf you "
          "see this message, please make "
          "sure your device is working properly.\nexiting...")
}
