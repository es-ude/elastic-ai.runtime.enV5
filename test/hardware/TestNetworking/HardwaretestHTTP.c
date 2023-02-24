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

    HttpResponse_t *response;
    uint8_t code = HTTPGet("http://192.168.203.99:5000/getfile/0", &response);

    PRINT("HTTPGet response: %d\n", code);
    PRINT("len %lu\n", response->length);
    for (uint32_t i = 0; i < response->length; i++) {
        printf("%c", response->response[i]);
    }
    printf("\n");
    HTTPCleanResponseBuffer(response);
    PRINT("done")

    while (1) {}
}

int main() {
    initHardwareTest();
    freeRtosTaskWrapperRegisterTask(enterBootModeTaskHardwareTest, "enterBootModeTask");
    freeRtosTaskWrapperRegisterTask(httpTask, "httpTask");
    freeRtosTaskWrapperStartScheduler();

    return 0;
}