#include "TaskWrapper.h"
#include <pico/bootrom.h>
#include <pico/stdlib.h>
#include <stdint.h>
#include <stdio.h>

uint8_t led1Pin = 22;
uint8_t led2Pin = 24;

void blink_led1() {
    // initialize LED
    gpio_init(led1Pin);
    gpio_set_dir(led1Pin, GPIO_OUT);

    while (true) {
        // turn LED on
        gpio_put(led1Pin, 1);
        // sleep
        TaskSleep(1000);
        // turn LED off
        gpio_put(led1Pin, 0);
        // sleep
        TaskSleep(1000);
    }
}

void blink_led2() {
    // initialize LED
    gpio_init(led2Pin);
    gpio_set_dir(led2Pin, GPIO_OUT);

    while (true) {
        // sleep
        TaskSleep(2500);
        // turn LED on
        gpio_put(led2Pin, 1);
        // sleep
        TaskSleep(2500);
        // turn LED off
        gpio_put(led2Pin, 0);
    }
}

int main(void) {
    // setup usb for debug output
    stdio_init_all();
    sleep_ms(1000);

    printf("Creating Task 01:\n");
    RegisterTask(blink_led1, "blink_led1");

    printf("Creating Task 02:\n");
    RegisterTask(blink_led2, "blink_led2");

    // start Tasks
    StartScheduler();
}
