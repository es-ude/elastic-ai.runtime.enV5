#include "FreeRtosMutexWrapper.h"
#include "Common.h"
#include "FreeRTOS.h"
#include "FreeRtosTaskWrapper.h"

#include <stdbool.h>

mutex_t freeRtosMutexWrapperCreate(void) {
    mutex_t mutex = xSemaphoreCreateMutex();
    if (NULL == mutex) {
        PRINT("Failed to create Mutex!");
    }
    return mutex;
}

void freeRtosMutexWrapperLock(mutex_t mutex) {
    while (pdTRUE != xSemaphoreTake(mutex, pdMS_TO_TICKS(10))) {
        freeRtosTaskWrapperTaskSleep(10);
    }
}
bool freeRtosMutexWrapperLockFromInterrupt(mutex_t mutex) {
    BaseType_t higherPriorityTaskWoken = pdFALSE;
    bool locked = xSemaphoreTakeFromISR(mutex, &higherPriorityTaskWoken);
    if (pdTRUE == higherPriorityTaskWoken) {
        taskYIELD();
    }
    return locked;
}
void freeRtosMutexWrapperUnlock(mutex_t mutex) {
    xSemaphoreGive(mutex);
}
void freeRtosMutexWrapperUnlockFromInterrupt(mutex_t mutex) {
    BaseType_t higherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(mutex, &higherPriorityTaskWoken);
    if (pdTRUE == higherPriorityTaskWoken) {
        taskYIELD();
    }
}
