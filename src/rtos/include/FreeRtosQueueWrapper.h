#ifndef ENV5_FREERTOS_QUEUE_WRAPPER_HEADER
#define ENV5_FREERTOS_QUEUE_WRAPPER_HEADER

#include <FreeRTOS.h>
#include <queue.h>
#include <stdbool.h>

typedef QueueHandle_t queue_t;

/*!
 * \brief create a queue
 *
 * @param numberOfElements number of elements the queue can hold at max
 * @param bytesPerElement number of bytes each element has
 * @return reference to the queue
 */
queue_t freeRtosQueueWrapperCreate(uint8_t numberOfElements, size_t bytesPerElement);

/*!
 * \brief copies the referenced data to the queue
 *
 * @param queue references to the queue
 * @param data data to be copied to queue
 * @return true if successful, else false
 */
bool freeRtosQueueWrapperPush(queue_t queue, void *data);
/*!
 * \brief copies the referenced data to the queue
 *
 * @param queue references to the queue
 * @param data data to be copied to queue
 * @return true if successful, else false
 */
bool freeRtosQueueWrapperPushFromInterrupt(queue_t queue, void *data);

/*!
 * \brief copies the first item from the queue
 *
 * @param queue references to the queue
 * @param data buffer for the element from the queue
 * @return true if successful, else false
 */
bool freeRtosQueueWrapperPop(queue_t queue, void *data);

#endif /* ENV5_FREERTOS_QUEUE_WRAPPER_HEADER */
