/*!
 * This test aims to provide a simple way to prove multiple FreeRTOS tasks
 * can run at the same time and on the same core.
 * It creates two Tasks, that count up.
 * It then runs a test, where each value is compared to a third counter running inside the test.
 */

#define SOURCE_FILE "HARDWARE-TEST-FREERTOS-TASKS"

#include "unity.h"

#include "pico/bootrom.h"
#include "pico/stdlib.h"

#include "eai/Common.h"
#include "eai/hal/EnV5HwController.h"
#include "eai/rtos/FreeRtosTaskWrapper.h"

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"

uint16_t val1 = 0;
uint16_t val2 = 0;

bool countTask1NewValue = false;
bool countTask2NewValue = false;

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

void countTask1() {
    for (int i = 0; i < 10; i++) {
        val1 = i;
        countTask1NewValue = true;

        while (countTask1NewValue) {
            vTaskDelay(10);
        }
    }
}

void countTask2() {
    for (int i = 0; i < 10; i++) {
        val2 = i;
        countTask2NewValue = true;

        while (countTask2NewValue) {
            vTaskDelay(10);
        }
    }
}

void testBothTaskValuesEqualExternalCounter() {
    bool bothTaskValuesEqualExternalCounter = true;

    for (int i = 0; i < 10; i++) {
        while (!countTask1NewValue || !countTask2NewValue) {
            vTaskDelay(10);
        }
        PRINT_DEBUG("val1 = %d, val2 = %d\n, i = %d", val1, val2, i);
        if (i != val1 || i != val2) {
            bothTaskValuesEqualExternalCounter = false;
            break;
        }
        countTask1NewValue = false;
        countTask2NewValue = false;
    }

    TEST_ASSERT_EQUAL(true, bothTaskValuesEqualExternalCounter);
}

void testRunnerTask() {
    freeRtosTaskWrapperRegisterTask(countTask1, "count_task_1", 0, FREERTOS_CORE_0);
    freeRtosTaskWrapperRegisterTask(countTask2, "count_task_2", 0, FREERTOS_CORE_0);

    UNITY_BEGIN();
    RUN_TEST(testBothTaskValuesEqualExternalCounter);
    UNITY_END();
    deInit();
}

int main(void) {
    init();
    freeRtosTaskWrapperRegisterTask(testRunnerTask, "test_runner_task", 0, FREERTOS_CORE_0);
    freeRtosTaskWrapperStartScheduler();
}
