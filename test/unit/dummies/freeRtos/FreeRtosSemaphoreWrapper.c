#include <semaphore.h>
#include <stdlib.h>

#include "FreeRtosSemaphoreWrapper.h"

Semaphore SemaphoreCreateBinary(void) {
    sem_t *sem = malloc(sizeof(sem_t));
    return sem;
}

void SemaphoreDelete(Semaphore semaphore) {}

bool SemaphoreGiveFromISR(Semaphore semaphore) {
    return true;
}

bool SemaphoreTake(Semaphore semaphore, int timeoutMs) {
    return true;
}