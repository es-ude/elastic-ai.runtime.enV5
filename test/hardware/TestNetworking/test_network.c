#define SOURCE_FILE "NETWORK-TEST"

#include "hardwareTestHelper.h"
#include "common.h"
#include "TaskWrapper.h"
#include "Network.h"
#include "tcp.h"
#include <string.h>
#include <stdio.h>

_Noreturn void ConnectAndSendTCPData() {
    connectToNetwork();

    char *responseBuf;
    char *cmd = "GET / HTTP/1.1\nHost: www.uni-due.de";

    while (true) {
        TCP_Open("www.uni-due.de", 80);
        TCP_SendData(cmd, 5000);

        if (NetworkStatus.TCPStatus == CONNECTED) {
            responseBuf = TCP_GetResponse();
            if (strlen(responseBuf) != 0) {
                PRINT("TCP Response: %s", responseBuf);
            }
        }

        TaskSleep(2000);
        TCP_Close(true);
    }
}

int main() {
    initHardwareTest();
    RegisterTask(ConnectAndSendTCPData, "ConnectAndSendTCPData");
    RegisterTask(enterBootModeTaskHardwareTest, "enterBootModeTask");
    StartScheduler();
}
