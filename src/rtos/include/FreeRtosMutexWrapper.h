#ifndef ENV5_FREERTOS_MUTEX_WRAPPER_HEADER
#define ENV5_FREERTOS_MUTEX_WRAPPER_HEADER

// needed to satisfy the compiler
#include "FreeRTOS.h"
#include "semphr.h"

#include <stdbool.h>

typedef SemaphoreHandle_t mutex_t;

/*!
 * \brief create a mutex
 *
 * @return reference to a mutex if successful, NULL otherwise
 */
mutex_t freeRtosMutexWrapperCreate(void);

/*!
 * \brief lock mutex
 *
 * @param mutex mutex to be locked
 */
void freeRtosMutexWrapperLock(mutex_t mutex);
/*!
 * \brief lock mutex from interrupt
 *
 * \IMPORTANT If you need to use this function, you maybe want to think about a task with another
 * queue!
 *
 * @param mutex mutex to be locked
 * @return true if mutex is locked, false otherwise
 */
bool freeRtosMutexWrapperLockFromInterrupt(mutex_t mutex);
/*!
 * \brief unlock mutex
 *
 * @param mutex mutex to be unlocked
 */
void freeRtosMutexWrapperUnlock(mutex_t mutex);
/*!
 * \brief unlock mutex from interrupt
 *
 * \IMPORTANT If you need to use this function, you maybe want to think about a task with another
 * queue!
 *
 * @param mutex mutex to be unlocked
 */
void freeRtosMutexWrapperUnlockFromInterrupt(mutex_t mutex);

#endif // ENV5_FREERTOS_MUTEX_WRAPPER_HEADER
