typedef void (*TaskCodeFunc)();

// registers new Task to be executed
void RegisterTask(TaskCodeFunc taskCode, const char *const taskName) {}

// sleep for ms
// DO NOT USE sleep_ms() IN TASKS!!!
void TaskSleep(int timeInMs) {}

// start executing all registered Tasks
void StartScheduler(void) {}
