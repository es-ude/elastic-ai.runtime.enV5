#define SOURCE_FILE "MAIN"
#define NDEBUG

#include <stdio.h>


#include "TaskWrapper.h"
#include "QueueWrapper.h"
#include <stdint.h>
//#include "Network.h"
#include "pico/bootrom.h"
#include "pico/stdlib.h"
#include "hardware/watchdog.h"
//#include "MQTTBroker.h"
#include "common.h"

static const uint8_t REG_DEVID = 0x00;


void enterBootModeTask(void);

void init(void);

void mainTask(void) {

    while (true) {
        gpio_init(18);
        gpio_set_dir(18, 1);
        gpio_put(18, 1);
               PRINT("Hello, World!");
        TaskSleep(5000);
        gpio_put(18, 0);
        TaskSleep(1000);

    }

}

int main() {
    init();

    RegisterTask(enterBootModeTask, "enterBootModeTask");
    RegisterTask((TaskCodeFunc) mainTask, "mainTask");
    StartScheduler();
}

void init(void) {
    // Did we crash last time -> reboot into boot rom mode
    if (watchdog_enable_caused_reboot()) {
        reset_usb_boot(0, 0);
    }



    // init usb, queue and watchdog
    stdio_init_all();
    while ((!stdio_usb_connected())) {}
    CreateQueue();
    watchdog_enable(2000, 1);
    //   while (!Network_init());

}

void _Noreturn enterBootModeTask(void) {
    while (true) {
        if (getchar_timeout_us(10) == 'r' || !stdio_usb_connected()) {

            //    MQTT_Broker_Disconnect(true);
            reset_usb_boot(0, 0);
        }
        watchdog_update();
        TaskSleep(1000);
    }
}
