#include "Common.h"
#include "FreeRtosTaskWrapper.h"
#include <pico/bootrom.h>
#include <pico/stdlib.h>

uint8_t led1Pin = 22;
uint8_t led2Pin = 24;

_Noreturn void blinkLed1Task() {
    // initialize LED
    gpio_init(led1Pin);
    gpio_set_dir(led1Pin, GPIO_OUT);

    while (1) {
        // turn LED on
        gpio_put(led1Pin, 1);
        // sleep
        freeRtosTaskWrapperTaskSleep(1000);
        // turn LED off
        gpio_put(led1Pin, 0);
        // sleep
        freeRtosTaskWrapperTaskSleep(1000);
    }
}

_Noreturn void blinkLed2Task() {
    // initialize LED
    gpio_init(led2Pin);
    gpio_set_dir(led2Pin, GPIO_OUT);

    while (1) {
        // turn LED on
        gpio_put(led2Pin, 1);
        // sleep
        freeRtosTaskWrapperTaskSleep(1500);
        // turn LED off
        gpio_put(led2Pin, 0);
        // sleep
        freeRtosTaskWrapperTaskSleep(1500);
    }
}

_Noreturn void enterBootModeTask() {
    while (1) {
        // enter boot mode if selected
        if (getchar_timeout_us(10) == 'r' || !stdio_usb_connected()) {
            reset_usb_boot(0, 0);
        }
    }
}

void initHardware() {
    // init usb and wait for user connection
    stdio_init_all();
    while ((!stdio_usb_connected())) {}
    freeRtosTaskWrapperTaskSleep(1000);
}

int main(void) {
    initHardware();

    PRINT("===== START TEST =====")
    // create Task1
    freeRtosTaskWrapperRegisterTask(blinkLed1Task, "blinkLed1Task", 0, 0);
    // createTask2
    freeRtosTaskWrapperRegisterTask(blinkLed2Task, "blinkLed2Task", 0, 0);
    // create boot mode task
    freeRtosTaskWrapperRegisterTask(enterBootModeTask, "enterBootMode", 0, 0);
    // start tasks
    freeRtosTaskWrapperStartScheduler();
}
