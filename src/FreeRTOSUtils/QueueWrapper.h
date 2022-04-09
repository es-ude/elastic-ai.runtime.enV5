#ifndef SENSOR_BOARD_QUEUE_WRAPPER_H
#define SENSOR_BOARD_QUEUE_WRAPPER_H

#include <stdbool.h>

#define QUEUE_LENGTH 10
#define QUEUE_WAIT_IF_BLOCKED_MS_AMOUNT 10
#define QUEUE_WAIT_FOR_RECEIVE_MS_AMOUNT 1000

typedef struct {
    // be aware that only the pointer is copied and not the content of the message!
    // see the queue hardware test for usage
    char *Data;
} QueueMessage;

void CreateQueue();

bool QueueSend(QueueMessage message);

bool QueueReceive(QueueMessage *message);

#endif // SENSOR_BOARD_QUEUE_WRAPPER_H
