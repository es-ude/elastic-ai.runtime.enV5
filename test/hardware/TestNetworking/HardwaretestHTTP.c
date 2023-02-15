#define SOURCE_FILE "HTTP-TEST"

#include <stdlib.h>
#include "Common.h"
#include "FreeRtosTaskWrapper.h"
#include "HardwaretestHelper.h"
#include "HTTP.h"
#include "string.h"

void _Noreturn httpTask(void) {
    PRINT("=== STARTING TEST ===")
    
    connectToNetwork();
    for(uint8_t j=0; j<20; j++) {
        char *response;

        uint8_t code = HTTPGet("http://google.com", &response);

        PRINT("returns with %d\n", code);
        PRINT("len %d\n", strlen(response));

        for (uint32_t i = 0; i < strlen(response); i++) {
            printf("%c", response[i]);
            if (i % 69 == 0) {
                printf("\n");
            }
        }
        printf("\n");
        free(response);

        PRINT("done\n");
    }
    while(1);
    
}

int main() {
    initHardwareTest();
    freeRtosTaskWrapperRegisterTask(enterBootModeTaskHardwareTest, "enterBootModeTask");
    freeRtosTaskWrapperRegisterTask(httpTask, "httpTask");
    freeRtosTaskWrapperStartScheduler();
}