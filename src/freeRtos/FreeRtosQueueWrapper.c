#define SOURCE_FILE "QUEUE-WRAPPER"

#include "FreeRTOS.h"
#include "FreeRtosQueueWrapper.h"
#include "Common.h"
#include <queue.h>

#define FREERTOS_QUEUE_WRAPPER_ITEM_SIZE sizeof(freeRtosQueueWrapperMessage_t)

static QueueHandle_t freeRtosQueueWrapperQueue;

void freeRtosQueueWrapperCreate() {
    freeRtosQueueWrapperQueue =
        xQueueCreate(FREERTOS_QUEUE_WRAPPER_QUEUE_LENGTH, FREERTOS_QUEUE_WRAPPER_ITEM_SIZE);
    if (freeRtosQueueWrapperQueue == NULL) {
        PRINT("Failed to create Message Queue! Communication between tasks not "
              "possible")
    }
}

bool freeRtosQueueWrapperSend(freeRtosQueueWrapperMessage_t message) {
    if (xQueueGenericSend(freeRtosQueueWrapperQueue, &message,
                          pdMS_TO_TICKS(FREERTOS_QUEUE_WRAPPER_WAIT_IF_BLOCKED_MS_AMOUNT),
                          queueSEND_TO_BACK) != pdPASS) {
        PRINT("Queue full!")
        return false;
    }
    return true;
}

bool freeRtosQueueWrapperReceive(freeRtosQueueWrapperMessage_t *message) {
    return xQueueReceive(freeRtosQueueWrapperQueue, message,
                         pdMS_TO_TICKS(FREERTOS_QUEUE_WRAPPER_WAIT_FOR_RECEIVE_MS_AMOUNT)) ==
           pdPASS;
}
