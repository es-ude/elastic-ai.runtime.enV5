#define SOURCE_FILE "HTTP-FLASH-TEST"

#include "Common.h"
#include "FpgaConfigurationHttp.h"
#include "FreeRtosTaskWrapper.h"
#include "HTTP.h"
#include "HardwaretestHelper.h"
#include "string.h"
#include <stdlib.h>

HttpResponse_t *getResponse(uint32_t block_number);

void _Noreturn httpTask(void) {
    PRINT("=== STARTING TEST ===")

    connectToNetwork();

    setCommunication(getResponse);
    configure(0, 205272);
    PRINT("done")

    while (1) {}
}

HttpResponse_t *getResponse(uint32_t block_number) {
    // todo: deep copy response and return this instead of the response pointer

    HttpResponse_t *response;
    char baseUrl[] = "http://192.168.178.25:5000/getfile/%u";
    char *URL = malloc(strlen(baseUrl) + block_number);
    sprintf(URL, baseUrl, block_number);

    uint8_t code = HTTPGet(URL, &response);

    PRINT_DEBUG("HTTP Get returns with %u", code);
    PRINT_DEBUG("Response Length: %li", response->length)
    PRINT_DEBUG("Response: %s", response->response)

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
