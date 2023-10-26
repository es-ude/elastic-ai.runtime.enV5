#define SOURCE_FILE "HTTP-TEST"

#include "CException.h"
#include "Common.h"
#include "FreeRtosTaskWrapper.h"
#include "HTTP.h"
#include "HardwaretestHelper.h"

void _Noreturn httpTask(void) {
    connectToNetwork();

    PRINT("=== STARTING TEST ===")

    HttpResponse_t *response = NULL;
    CEXCEPTION_T exception;

    while (1) {

        Try {
            HTTPGet("http://192.168.178.24:5000/check", &response);
            PRINT("HTPPGet Success!\n\tResponse Length: %li\n\tResponse: %s", response->length,
                  response->response)
            HTTPCleanResponseBuffer(response);
        }
        Catch(exception){PRINT("HTTPGet failed!")}

        freeRtosTaskWrapperTaskSleep(3000);
    }
}

int main() {
    initHardwareTest();
    freeRtosTaskWrapperRegisterTask(enterBootModeTaskHardwareTest, "enterBootModeTask", 0,
                                    FREERTOS_CORE_0);
    freeRtosTaskWrapperRegisterTask(httpTask, "httpTask", 0, FREERTOS_CORE_0);
    freeRtosTaskWrapperStartScheduler();

    return 0;
}
