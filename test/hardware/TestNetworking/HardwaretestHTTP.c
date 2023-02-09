#define SOURCE_FILE "HTTP-TEST"

#include "Common.h"
#include "FreeRtosTaskWrapper.h"
#include "HardwaretestHelper.h"
#include "HTTP.h"

void _Noreturn httpTask(void) {
    PRINT("=== STARTING TEST ===")
    
    connectToNetwork();
    
    char *response;

    uint8_t code = HTTPGet("http://httpbin.org/get", &response);
    
    PRINT("%d\n", code);
    
    while(1);
    
}

int main() {
    initHardwareTest();
    freeRtosTaskWrapperRegisterTask(enterBootModeTaskHardwareTest, "enterBootModeTask");
    freeRtosTaskWrapperRegisterTask(httpTask, "httpTask");
    freeRtosTaskWrapperStartScheduler();
}