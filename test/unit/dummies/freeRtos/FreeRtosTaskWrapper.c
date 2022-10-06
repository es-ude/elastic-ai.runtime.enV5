typedef void (*TaskCodeFunc)();

// registers new Task to be executed
void freeRtosTaskWrapperRegisterTask(TaskCodeFunc taskCode, const char *taskName) {}

// sleep for ms
// DO NOT USE sleep_ms() IN TASKS!!!
void freeRtosTaskWrapperTaskSleep(int timeInMs) {}

// start executing all registered Tasks
void freeRtosTaskWrapperStartScheduler(void) {}
