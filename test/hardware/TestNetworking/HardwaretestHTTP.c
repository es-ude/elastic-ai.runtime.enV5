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
    
//
//    char *baseURL = "http://192.168.0.216:5000/getfile?pos=%d";
//    char *wholeurl = malloc(sizeof (char) * (strlen(baseURL) + 3));
    //char *url = "http://192.168.0.216:5000/getfile";
//    sprintf(wholeurl, baseURL, 100);
//    PRINT_DEBUG(wholeurl);
    uint8_t code = HTTPGet("http://192.168.0.216:5000/getfile/0", &response);
   // uint8_t code = HTTPGet("http://httpbin.org/get", &response);
    PRINT("returns with %d\n", code);
    // PRINT("len %d\n", strlen(response));

    for (uint32_t i = 0; i < 256; i++) {
        printf("%c", response[i]);
        if (i % 69 == 0) {
            printf("\n");
        }
    }
    printf("\n");
    free(response);

    PRINT("done\n");

    while (1)
        ;
}

int main() {
    initHardwareTest();
    freeRtosTaskWrapperRegisterTask(enterBootModeTaskHardwareTest, "enterBootModeTask");
    freeRtosTaskWrapperRegisterTask(httpTask, "httpTask");
    freeRtosTaskWrapperStartScheduler();
}