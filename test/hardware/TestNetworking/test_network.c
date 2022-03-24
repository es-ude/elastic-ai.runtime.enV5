#include "TaskWrapper.h"
#include "Network.h"
#include "stdio.h"
#include <stdlib.h>
#include <string.h>

#include "espMQTTBroker.h"

#include "espBase.h"

#include "pico/bootrom.h"
#include "pico/stdlib.h"
#include "hardware/watchdog.h"
#include "QueueWrapper.h"

#include "NetworkSettings.h"

_Noreturn void ConnectToAccessPointTask();

void initHardwareTest(void);

void _Noreturn enterBootModeTaskHardwareTest(void);

int main() {
    initHardwareTest();
    // connects to the ap
    RegisterTask(ConnectToAccessPointTask, "ConnectToAccessPointTask");
    // used to reset the pi if r is pressed
    RegisterTask(enterBootModeTaskHardwareTest, "enterBootModeTask");
    // GO!
    StartScheduler();
}

_Noreturn void ConnectToAccessPointTask() {
    Network_Init(false);

    while (Network_GetStatusFlags().WIFIStatus == NOT_CONNECTED)
        Network_ConnectToNetwork(credentials);

    char *responseBuf;
    char *cmd = "GET / HTTP/1.1\n";
    while (true) {
        Network_TCP_Open("www.uni-due.de", 80);
        if (Network_GetStatusFlags().TCPStatus == CONNECTED) {
            Network_TCP_SendData(cmd, 5000);
            responseBuf = Network_TCP_GetResponse();
        }
        if (responseBuf != 0) {
            printf("Got Response from library len:%d\n\n%s\n", strlen(responseBuf), responseBuf);
            free(responseBuf);
            TaskSleep(2000);
            Network_TCP_Close(false);
        }
    }
}

void initHardwareTest(void) {
    // Did we crash last time -> reboot into bootrom mode
    if (watchdog_enable_caused_reboot()) {
        reset_usb_boot(0, 0);
    }
    Network_Init(false);
    // init usb, queue and watchdog
    stdio_init_all();
    while ((!stdio_usb_connected())) {}
    CreateQueue();
    watchdog_enable(2000, 1);
}

void _Noreturn enterBootModeTaskHardwareTest(void) {
    while (true) {
        if (getchar_timeout_us(10) == 'r' || !stdio_usb_connected()) {
            ESP_MQTT_BROKER_Disconnect(true);
            reset_usb_boot(0, 0);
        }
        watchdog_update();
        TaskSleep(1000);
    }
}
