#define SOURCE_FILE "SEMAPHORE-WRAPPER"

#include "FreeRTOS.h"
#include "semphr.h"

#include "SemaphoreWrapper.h"

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
