#ifndef FREERTOS_SEMAPHORE_WRAPPER_H
#define FREERTOS_SEMAPHORE_WRAPPER_H

#include <stdbool.h>

typedef void *Semaphore;

Semaphore SemaphoreCreateBinary(void);

void SemaphoreDelete(Semaphore semaphore);

bool SemaphoreGiveFromISR(Semaphore semaphore);

bool SemaphoreTake(Semaphore semaphore, int timeoutMs);

#endif
