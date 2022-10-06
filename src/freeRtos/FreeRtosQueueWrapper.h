#ifndef ENV5_FREERTOS_QUEUE_WRAPPER_HEADER
#define ENV5_FREERTOS_QUEUE_WRAPPER_HEADER

#include <stdbool.h>

#define FREERTOS_QUEUE_WRAPPER_QUEUE_LENGTH 10
#define FREERTOS_QUEUE_WRAPPER_WAIT_IF_BLOCKED_MS_AMOUNT 10
#define FREERTOS_QUEUE_WRAPPER_WAIT_FOR_RECEIVE_MS_AMOUNT 1000

struct freeRtosQueueWrapperMessage {
    // be aware that only the pointer is copied and not the content of the
    // message! see the queue hardware test for usage
    char *Data;
};
typedef struct freeRtosQueueWrapperMessage freeRtosQueueWrapperMessage_t;

void freeRtosQueueWrapperCreate(void);

bool freeRtosQueueWrapperSend(freeRtosQueueWrapperMessage_t message);

bool freeRtosQueueWrapperReceive(freeRtosQueueWrapperMessage_t *message);

#endif /* ENV5_FREERTOS_QUEUE_WRAPPER_HEADER */
