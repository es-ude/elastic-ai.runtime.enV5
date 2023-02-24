#define SOURCE_FILE "HTTP-TEST"

#include "Common.h"
#include "FreeRtosTaskWrapper.h"
#include "HTTP.h"
#include "HardwaretestHelper.h"
#include "string.h"
#include <stdlib.h>

void _Noreturn httpTask(void) {
    PRINT("=== STARTING TEST ===")

    connectToNetwork();

    char *response;
    uint8_t code = HTTPGet("http://192.168.203.99:5000/getfile/0", &response);

    PRINT("HTTPGet response: %d\n", code);

    PRINT("len %d\n", strlen(response));
    for (uint32_t i = 0; i < 100; i++) {
        PRINT("%c", response[i])
    }
    free(response);

    PRINT("done\n")

    while (1) {}
}

int main() {
    initHardwareTest();
    freeRtosTaskWrapperRegisterTask(enterBootModeTaskHardwareTest, "enterBootModeTask");
    freeRtosTaskWrapperRegisterTask(httpTask, "httpTask");
    freeRtosTaskWrapperStartScheduler();

    return 0;
}