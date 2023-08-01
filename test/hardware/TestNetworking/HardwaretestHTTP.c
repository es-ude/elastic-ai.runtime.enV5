#define SOURCE_FILE "HTTP-TEST"

#include "Common.h"
#include "FreeRtosTaskWrapper.h"
#include "HTTP.h"
#include "HardwaretestHelper.h"

void _Noreturn httpTask(void) {
    connectToNetwork();

    PRINT("=== STARTING TEST ===")

    HttpResponse_t *response = NULL;

    while (1) {
        uint8_t code = HTTPGet("http://192.168.178.24:5000/check", &response);

        PRINT("HTTP Get returns with %u", code);
        if (code == HTTP_SUCCESS) {
            PRINT("Response Length: %li", response->length);
            PRINT("Response: %s", response->response)
            HTTPCleanResponseBuffer(&response);
        }

        freeRtosTaskWrapperTaskSleep(3000);
    }
}

int main() {
    initHardwareTest();
    freeRtosTaskWrapperRegisterTask(enterBootModeTaskHardwareTest, "enterBootModeTask");
    freeRtosTaskWrapperRegisterTask(httpTask, "httpTask");
    freeRtosTaskWrapperStartScheduler();

    return 0;
}
