#include "TaskWrapper.h"
#include <stdint.h>
#include <pico/stdlib.h>
#include <pico/bootrom.h>
#include <stdio.h>

void blink_red(void);
void blink_green(void);

int main(void) {
    // setup usb for debug output
    stdio_init_all();
    sleep_ms(1000);
    
    printf("Creating Task 01:\n");
    RegisterTask(blink_red, "blink_red");

    printf("Creating Task 02:\n");
    RegisterTask(blink_green, "blink_green");

    // start Tasks
    StartScheduler();
}

void blink_red() {
    // initialize LED
    uint8_t ledPin = 18;
    gpio_init(ledPin);
    gpio_set_dir(ledPin, GPIO_OUT);
    
    while (true) {
        // turn LED on
        gpio_put(ledPin, 1);
        // sleep
        TaskSleep(1000);
        // turn LED off
        gpio_put(ledPin, 0);
        // sleep
        TaskSleep(1000);
    }
}

void blink_green() {
    // initialize LED
    uint8_t ledPin = 19;
    gpio_init(ledPin);
    gpio_set_dir(ledPin, GPIO_OUT);
    
    while (true) {
        // sleep
        TaskSleep(1000);
        // turn LED on
        gpio_put(ledPin, 1);
        // sleep
        TaskSleep(1000);
        // turn LED off
        gpio_put(ledPin, 0);
    }
}
