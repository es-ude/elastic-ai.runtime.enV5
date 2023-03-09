#define SOURCE_FILE "HTTP-FLASH-TEST"

#include "Common.h"
#include "FpgaConfigurationHttp.h"
#include "FreeRtosTaskWrapper.h"
#include "HTTP.h"
#include "HardwaretestHelper.h"
#include "string.h"
#include <stdlib.h>
#include "Spi.h"
#include "Flash.h"
#include <hardware/spi.h>
#include "env5/Env5Hw.h"

static const uint8_t sck_pin = 2;
static const uint8_t miso_pin = 0;
static const uint8_t mosi_pin = 3;
static const uint8_t cs_pin = 1;
static const uint32_t baudrate = 5000 * 1000;


void init_helper(spi_inst_t *spi, uint32_t baudrate) {
    spiInit(spi, baudrate, cs_pin, sck_pin, mosi_pin, miso_pin);
    flashInit(cs_pin, spi);
}


HttpResponse_t *getResponse(uint32_t block_number);

void _Noreturn httpTask(void) {
    PRINT("=== STARTING TEST ===")

    connectToNetwork();
    spi_inst_t *spi = spi0;
    env5HwInit();
    init_helper(spi, baudrate);

    setCommunication(getResponse);
    configErrorCode_t configErrorCode=configure(0, 205272);
    if(configErrorCode==CONFIG_ERASE_ERROR){
        PRINT("ERASE ERROR")
    }
    PRINT("done")
    while(1);
}

HttpResponse_t *getResponse(uint32_t block_number) {
    // todo: deep copy response and return this instead of the response pointer

    HttpResponse_t *response;
    char baseUrl[] = "http://192.168.201.198:5000/getfile/%u";
    char *URL = malloc(strlen(baseUrl) + block_number);
    sprintf(URL, baseUrl, block_number);

    uint8_t code = HTTPGet(URL, &response);

    PRINT_DEBUG("HTTP Get returns with %u", code);
    PRINT_DEBUG("Response Length: %li", response->length)
    //PRINT_DEBUG("Response: %s", response->response)

   // HTTPCleanResponseBuffer(response);
    free(URL);
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
