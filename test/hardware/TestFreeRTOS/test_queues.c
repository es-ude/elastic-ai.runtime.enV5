#include "QueueWrapper.h"
#include "TaskWrapper.h"
#include <hardware/watchdog.h>
#include <pico/bootrom.h>
#include <pico/stdlib.h>
#include <stdio.h>
#include <string.h>

void SenderTask(void);

void MainTask(void);

int main(void) {
    // Did we crash last time -> reboot into boot rom mode
    if (watchdog_enable_caused_reboot()) {
        reset_usb_boot(0, 0);
    }
    // init usb and watchdog
    stdio_init_all();
    TaskSleep(1000);
    while ((!stdio_usb_connected())) {}
    watchdog_enable(2000, 1);
    CreateQueue();

    // test dynamic vs static
    printf("test test dynamic vs. static:\n");
    char str[10];
    strcpy(str, "hallo");
    QueueMessage msg;
    msg.Data = str;
    QueueSend(msg);
    msg.Data[0] = 'a';
    QueueReceive(&msg);
    printf("Received :%s\n", msg.Data);

    // test async
    printf("test async:\n");

    RegisterTask(SenderTask, "sensor");
    RegisterTask(MainTask, "main");
    StartScheduler();
}

_Noreturn void SenderTask() {
    QueueMessage msg;
    msg.Data = 0;
    while (true) {
        if (QueueSend(msg)) {
            printf("Send into Queue\n");
        } else {
            printf("Failed to Send to queue\n");
        }
        msg.Data++;
        TaskSleep(200);
    }
}

void MainTask() {
    QueueMessage msg;
    while (true) {
        if (!QueueReceive(&msg)) {
            printf("Something went Wrong!\n");
        } else {
            printf("Received: %d\n", msg.Data);
        }
        if (getchar_timeout_us(10) == 'r' || !stdio_usb_connected()) {
            reset_usb_boot(0, 0);
        }
        watchdog_update();
        TaskSleep(200);
    }
}
