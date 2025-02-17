#define SOURCE_FILE "HARDWARE-TEST-FREERTOS-TASKS"

#include "Common.h"
#include "EnV5HwController.h"

// needed to make compiler happy
#include "FreeRTOS.h"

#include "FreeRtosTaskWrapper.h"
#include "unity.h"

#include "pico/bootrom.h"
#include "pico/stdlib.h"

#include <task.h>

uint16_t val1 = 0;
uint16_t val2 = 0;

bool countTask1Updated = false;
bool countTask2Updated = false;

bool bothTaskValuesChecked = false;

TaskHandle_t checkValTaskHandle = NULL;
TaskHandle_t task1Handle = NULL;
TaskHandle_t task2Handle = NULL;

_Noreturn void countTask1() {
    for (int i = 0; i < 10; i++) {
        val1 = i;

        // needed for synchronization
        vTaskDelay(10);

        countTask1Updated = true;

        while (!bothTaskValuesChecked) {
            vTaskDelay(100);
        }
        countTask1Updated = false;
    }
}
_Noreturn void countTask2() {
    for (int i = 0; i < 10; i++) {
        val2 = i;

        // needed for synchronization
        vTaskDelay(10);

        countTask2Updated = true;

        while (!bothTaskValuesChecked) {
            vTaskDelay(100);
        }
        countTask2Updated = false;
    }
}

void init() {
    stdio_init_all();
    while (!stdio_usb_connected()) {}
    env5HwControllerInit();
}

void setUp(void) {}
void tearDown(void) {}
void deInit() {
    rom_reset_usb_boot(0, 0);
}

void checkBothTaskValuesEqualInternalCounter() {
    for (int i = 0; i < 10; i++) {
        bothTaskValuesChecked = false;
        if (countTask1Updated && countTask2Updated) {
            TEST_ASSERT_EQUAL(i, val1);
            TEST_ASSERT_EQUAL(i, val2);
        }
        bothTaskValuesChecked = true;
    }
}

void testRunnerTask() {
    freeRtosTaskWrapperRegisterTask(countTask1, "count_task_1", 0, FREERTOS_CORE_0);
    freeRtosTaskWrapperRegisterTask(countTask2, "count_task_2", 0, FREERTOS_CORE_0);
    UNITY_BEGIN();
    RUN_TEST(checkBothTaskValuesEqualInternalCounter);
    UNITY_END();
    deInit();
}

int main(void) {
    init();
    freeRtosTaskWrapperRegisterTask(testRunnerTask, "test_runner", 0, FREERTOS_CORE_0);
    freeRtosTaskWrapperStartScheduler();
}
