#define SOURCE_FILE "NETWORK-TEST"

#include "TaskWrapper.h"
#include "Network.h"
#include "tcp.h"
#include "stdio.h"
#include <stdlib.h>
#include <string.h>


#include "hardwareTestHelper.h"

_Noreturn void ConnectToAccessPointTask();

int main() {
    initHardwareTest();
    // connects to the ap
//    RegisterTask(ConnectToAccessPointTask, "ConnectToAccessPointTask");
    // used to reset the pi if r is pressed
    RegisterTask(enterBootModeTaskHardwareTest, "enterBootModeTask");
    // GO!
    StartScheduler();
}

_Noreturn void ConnectToAccessPointTask() {
    connectToNetwork();
    connectToMQTT();

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
