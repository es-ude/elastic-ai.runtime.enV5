//
// use this wrapper to crate new async Tasks in your project
// it also removes direct dependency on FreeRTOS
//

#ifndef FREERTOS_TASKWRAPPER_H
#define FREERTOS_TASKWRAPPER_H

typedef void (*TaskCodeFunc)();

// registers new Task to be executed
void RegisterTask(TaskCodeFunc taskCode, const char *const taskName);

// sleep for ms
// DO NOT USE sleep_ms() IN TASKS!!!
void TaskSleep(int timeInMs);

// start executing all registered Tasks
void StartScheduler(void);

#endif // FREERTOS_TASKWRAPPER_H
