#define SOURCE_FILE "QUEUE-WRAPPER"

#include "FreeRTOS.h"
#include "queue.h"

#include "eai/Common.h"
#include "eai/rtos/FreeRtosQueueWrapper.h"

#ifndef FREERTOS_QUEUE_WRAPPER_WAIT_IF_FULL
//! Time in milliseconds to wait if the queue is full
#define FREERTOS_QUEUE_WRAPPER_WAIT_IF_FULL 10
#endif

queue_t freeRtosQueueWrapperCreate(uint8_t numberOfElements, size_t bytesPerElement) {
    queue_t queue = xQueueCreate(numberOfElements, bytesPerElement);
    if (NULL == queue) {
        PRINT("Failed to create Message Queue!");
    }
    return queue;
}

bool freeRtosQueueWrapperPush(queue_t queue, void *data) {
    if (pdTRUE ==
        xQueueSendToBack(queue, data, pdMS_TO_TICKS(FREERTOS_QUEUE_WRAPPER_WAIT_IF_FULL))) {
        return true;
    }
    return false;
}

bool freeRtosQueueWrapperPushFromInterrupt(queue_t queue, void *data) {
    BaseType_t higherPriorityTaskWoken = pdFALSE;
    if (pdPASS == xQueueSendToBackFromISR(queue, data, &higherPriorityTaskWoken)) {
        return true;
    }
    if (pdTRUE == higherPriorityTaskWoken) {
        taskYIELD();
    }
    return false;
}

bool freeRtosQueueWrapperPop(queue_t queue, void *data) {
    if (pdTRUE == xQueueReceive(queue, data, 0)) {
        return true;
    }
    return false;
}
