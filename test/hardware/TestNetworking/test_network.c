#define SOURCE_FILE "NETWORK-TEST"

#include "TaskWrapper.h"
#include "Network.h"
#include "tcp.h"
#include "stdio.h"
#include <stdlib.h>
#include <string.h>

#include "MQTTBroker.h"

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
    Network_init(false);

    while (NetworkStatus.WIFIStatus == NOT_CONNECTED)
        Network_ConnectToNetwork(credentials);

    char *responseBuf;
    char *cmd = "GET / HTTP/1.1\n";
    while (true) {
        TCP_Open("www.uni-due.de", 80);
        if (NetworkStatus.TCPStatus == CONNECTED) {
            TCP_SendData(cmd, 5000);
            responseBuf = TCP_GetResponse();
            if (responseBuf != 0) {
                printf("Got Response from library len:%d\n\n%s\n", strlen(responseBuf), responseBuf);
                free(responseBuf);
                TaskSleep(2000);
                TCP_Close(false);
            }
        }
    }
}

void initHardwareTest(void) {
    // Did we crash last time -> reboot into boot rom mode
    if (watchdog_enable_caused_reboot()) {
        reset_usb_boot(0, 0);
    }
    Network_init(false);
    // init usb, queue and watchdog
    stdio_init_all();
    while ((!stdio_usb_connected())) {}
    CreateQueue();
    watchdog_enable(2000, 1);
}

void _Noreturn enterBootModeTaskHardwareTest(void) {
    while (true) {
        if (getchar_timeout_us(10) == 'r' || !stdio_usb_connected()) {
            MQTT_Broker_Disconnect(true);
            reset_usb_boot(0, 0);
        }
        watchdog_update();
        TaskSleep(1000);
    }
}
