#ifndef ENV5_FREERTOS_TASK_WRAPPER_HEADER
#define ENV5_FREERTOS_TASK_WRAPPER_HEADER

#include <stdint.h>

#define FREERTOS_CORE_0 (1 << 0)
#define FREERTOS_CORE_1 (1 << 1)

typedef void (*TaskCodeFunc)();

/*!
 * @brief registers new Task to be executed
 *
 * @param taskCode pointer to function which should be executed as the task
 * @param taskName name which references the task
 * @param prio priority which allows the task prioritization
 *             higher number mean greater priority
 * @param core Either `FREERTOS_CORE_0` or `FREERTOS_CORE_1`
 */
void freeRtosTaskWrapperRegisterTask(TaskCodeFunc taskCode, const char *taskName, uint8_t prio,
                                     uint8_t core);

/*!
 * @brief make the task sleep
 *
 * @IMPORTANT DO NOT USE sleep_ms() IN TASKS!
 *
 * @param timeInMs time to sleep in milliseconds
 *                 minimal supported time: 10ms
 */
void freeRtosTaskWrapperTaskSleep(int timeInMs);

/*!
 * @brief start executing all registered Tasks
 */
void freeRtosTaskWrapperStartScheduler(void);

#endif /* ENV5_FREERTOS_TASK_WRAPPER_HEADER */
