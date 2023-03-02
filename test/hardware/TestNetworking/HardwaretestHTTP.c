#define SOURCE_FILE "HTTP-TEST"

#include "Common.h"
#include "FreeRtosTaskWrapper.h"
#include "HTTP.h"
#include "HardwaretestHelper.h"

void _Noreturn httpTask(void) {
    PRINT("=== STARTING TEST ===")

    connectToNetwork();

    HttpResponse_t *response;
    uint8_t code = HTTPGet("http://192.168.178.24:5000/getfile/0", &response);

    PRINT("HTTP Get returns with %u", code);
    PRINT("Response Length: %li", response->length);
    PRINT("Response: %s", response->response)

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