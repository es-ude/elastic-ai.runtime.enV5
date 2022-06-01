#define SOURCE_FILE "NETWORK-TEST"

#include "hardwareTestHelper.h"
#include "common.h"
#include "TaskWrapper.h"
#include "Network.h"
#include <string.h>
#include <stdio.h>

int main() {
    initHardwareTest();
    RegisterTask(enterBootModeTaskHardwareTest, "enterBootModeTask");
    StartScheduler();
}
