#include "TaskWrapper.h"
#include "Network.h"
#include "pico/bootrom.h"
#include "pico/stdlib.h"
#include "stdio.h"
#include <stdlib.h>
#include <string.h>
#include <hardware/watchdog.h>

/* FIXME: update wifi credentials and MQTT Host IP/Port */
NetworkCredentials credentials = {
    .ssid     = "wifi_network",
    .password = "wifi_password"};

void TestEnterBootModeTask();

_Noreturn void ConnectToAccessPointTask();

int main() {
    // Enable usb so we can print status output
    stdio_init_all();
    TaskSleep(1000);
    if(watchdog_enable_caused_reboot())
        reset_usb_boot(0,0);
    while ((!stdio_usb_connected())) {}
    printf("test_network_library:\n");
    // connects to the ap
    RegisterTask(ConnectToAccessPointTask, "ConnectToAccessPointTask");
    // used to reset the pi if r is pressed
    RegisterTask(TestEnterBootModeTask, "TestEnterBootModeTask");
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
        if(responseBuf != 0) {
            printf("Got Response from library len:%d\n\n%s\n", strlen(responseBuf), responseBuf);
            free(responseBuf);
            TaskSleep(2000);
            Network_TCP_Close(false);
        }
    }
}

void TestEnterBootModeTask() {

    watchdog_enable(2000, 1);
    while (true) {
        if (getchar_timeout_us(10) == 'r' || !stdio_usb_connected()) {
            Network_TCP_Close(true);
            reset_usb_boot(0, 0);
        }
        TaskSleep(1000);
        watchdog_update();
    }
}
