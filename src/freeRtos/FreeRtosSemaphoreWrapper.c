#define SOURCE_FILE "SEMAPHORE-WRAPPER"

#define INCLUDE_ENV5_FREERTOS_TASKWRAPPER_INTERNAL_HEADER
#include "FreeRtosSemaphoreWrapper.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include <stdbool.h>

Semaphore SemaphoreCreateBinary(void) {
    return xSemaphoreCreateBinary();
}

void SemaphoreDelete(Semaphore semaphore) {
    vSemaphoreDelete(semaphore);
}

bool SemaphoreGiveFromISR(Semaphore semaphore) {
    return xSemaphoreGiveFromISR(semaphore, NULL) == pdTRUE;
}

bool SemaphoreTake(Semaphore semaphore, int timeoutMs) {
    return xSemaphoreTake(semaphore, pdMS_TO_TICKS(timeoutMs)) == pdPASS;
}
