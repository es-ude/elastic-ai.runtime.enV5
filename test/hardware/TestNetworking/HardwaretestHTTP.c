#define SOURCE_FILE "HTTP-TEST"

#include "Common.h"
#include "FreeRtosTaskWrapper.h"
#include "HTTP.h"
#include "HardwaretestHelper.h"
#include "FpgaConfigurationHttp.h"
#include "string.h"
#include <stdlib.h>

HttpResponse_t* getResponse(uint32_t block_number);


void _Noreturn httpTask(void) {
    PRINT("=== STARTING TEST ===")

    connectToNetwork();

    setCommunication(getResponse);
    configure(0, 205272);
//    uint8_t code = HTTPGet("http://192.168.203.99:5000/getfile/0", &response);
//
//    PRINT("HTTPGet response: %d\n", code);
//    PRINT("len %lu\n", response->length);
//    for (uint32_t i = 0; i < response->length; i++) {
//        printf("%c", response->response[i]);
//    }
//    printf("\n");
//    HTTPCleanResponseBuffer(response);
//    PRINT("done")

    while (1) {}
}

HttpResponse_t* getResponse(uint32_t block_number) {
    // todo: deep copy response and return this instead of the response pointer
    
    HttpResponse_t *response;
    char baseUrl[] = "http://192.168.203.99:5000/getfile/%u";
    char* URL = malloc(strlen(baseUrl) + block_number);
    sprintf(URL, baseUrl, block_number);
    
    uint8_t code = HTTPGet(URL, &response);

    PRINT_DEBUG("HTTP Get returns with %u", code);
    
    HTTPCleanResponseBuffer(response);
    PRINT("done")
    return response;
}

int main() {
    initHardwareTest();
    freeRtosTaskWrapperRegisterTask(enterBootModeTaskHardwareTest, "enterBootModeTask");
    freeRtosTaskWrapperRegisterTask(httpTask, "httpTask");
    freeRtosTaskWrapperStartScheduler();

    return 0;
}