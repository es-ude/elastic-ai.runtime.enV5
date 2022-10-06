#ifndef FREERTOS_TASK_WRAPPER_H
#define FREERTOS_TASK_WRAPPER_H

/***
// use this wrapper to crate new async Tasks in your project
// it also removes direct dependency on FreeRTOS
***/

typedef void (*TaskCodeFunc)();

// registers new Task to be executed
void RegisterTask(TaskCodeFunc taskCode, const char *taskName);

// sleep for ms
// DO NOT USE sleep_ms() IN TASKS!!!
void TaskSleep(int timeInMs);

// start executing all registered Tasks
void StartScheduler(void);

#endif // FREERTOS_TASK_WRAPPER_H
