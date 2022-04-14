#define SOURCE_FILE "QUEUE-WRAPPER"

#include "common.h"
#include "QueueWrapper.h"
#include "FreeRTOS.h"
#include "queue.h"

#define QUEUE_ITEM_SIZE sizeof(QueueMessage)
static QueueHandle_t queue;

void CreateQueue() {
    queue = xQueueCreate(QUEUE_LENGTH, QUEUE_ITEM_SIZE);
    if (queue == NULL) {
        PRINT("Failed to create Message Queue! Communication between tasks not possible")
    }
}

bool QueueSend(QueueMessage message) {
    if (xQueueGenericSend(queue, &message, pdMS_TO_TICKS(QUEUE_WAIT_IF_BLOCKED_MS_AMOUNT),
                          queueSEND_TO_BACK) != pdPASS) {
        PRINT("Queue full!")
        return false;
    }
    return true;
}

bool QueueReceive(QueueMessage *message) {
    return xQueueReceive(queue, message, pdMS_TO_TICKS(QUEUE_WAIT_FOR_RECEIVE_MS_AMOUNT)) == pdPASS;
}
