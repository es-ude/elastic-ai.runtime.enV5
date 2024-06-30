/*!
 * This test aims to provide a simple way to prove FreeRTOS tasks can be compiled and executed
 */

#define SOURCE_FILE "HARDWARE-TEST-FREERTOS-TASKS"

#include "Common.h"
#include "EnV5HwConfiguration.h"
#include "EnV5HwController.h"
#include "FreeRtosTaskWrapper.h"
#include "Gpio.h"

#include "hardware/watchdog.h"
#include "pico/bootrom.h"
#include "pico/stdlib.h"

_Noreturn void blinkLed1Task() {
    while (1) {
        PRINT_DEBUG("BLINK 1");
        gpioSetPin(LED0_GPIO, GPIO_PIN_HIGH); // turn LED on
        freeRtosTaskWrapperTaskSleep(1000);
        gpioSetPin(LED1_GPIO, GPIO_PIN_LOW); // turn LED off
        freeRtosTaskWrapperTaskSleep(1000);
    }
}

_Noreturn void blinkLed2Task() {
    while (1) {
        PRINT_DEBUG("BLINK 2");
        gpioSetPin(LED2_GPIO, GPIO_PIN_HIGH); // turn LED on
        freeRtosTaskWrapperTaskSleep(1500);
        gpioSetPin(LED2_GPIO, GPIO_PIN_LOW); // turn LED off
        freeRtosTaskWrapperTaskSleep(1500);
    }
}

_Noreturn void enterBootModeTask() {
    while (1) {
        PRINT_DEBUG("RESET WATCHDOG");
        watchdog_update();
        freeRtosTaskWrapperTaskSleep(2000);
    }
}

void initHardware() {
    // check if we crash last time -> reboot into boot rom mode
    if (watchdog_enable_caused_reboot()) {
        reset_usb_boot(0, 0);
    }

    env5HwControllerInit();

    // initialize the serial output
    stdio_init_all();
    while ((!stdio_usb_connected())) {
        // wait for serial connection
    }

    // enables watchdog timer (5s)
    watchdog_enable(5000, 1);
}

int main(void) {
    initHardware();

    freeRtosTaskWrapperRegisterTask(blinkLed1Task, "blinkLed1Task", 0, FREERTOS_CORE_0);
    freeRtosTaskWrapperRegisterTask(blinkLed2Task, "blinkLed2Task", 0, FREERTOS_CORE_0);
    freeRtosTaskWrapperRegisterTask(enterBootModeTask, "enterBootMode", 0, FREERTOS_CORE_0);
    freeRtosTaskWrapperStartScheduler();
}
