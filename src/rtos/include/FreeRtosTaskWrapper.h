#ifndef ENV5_FREERTOS_TASK_WRAPPER_HEADER
#define ENV5_FREERTOS_TASK_WRAPPER_HEADER

#define FREERTOS_CORE_0 (1 << 0)
#define FREERTOS_CORE_1 (1 << 1)

#include <stdint.h>
/*!
 *  use this wrapper to create new async Tasks in your project
 * it also removes direct dependency on FreeRTOS
 */

typedef void (*TaskCodeFunc)();

// registers new Task to be executed
void freeRtosTaskWrapperRegisterTask(TaskCodeFunc taskCode, const char *taskName, uint8_t prio,
                                     uint8_t core);

/*! DO NOT USE sleep_ms() IN TASKS!!! */
void freeRtosTaskWrapperTaskSleep(int timeInMs);

// start executing all registered Tasks
void freeRtosTaskWrapperStartScheduler(void);

#endif /* ENV5_FREERTOS_TASK_WRAPPER_HEADER */
