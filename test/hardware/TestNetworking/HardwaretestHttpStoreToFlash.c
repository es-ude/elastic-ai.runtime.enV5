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
#include "pico/stdio.h"
#include "middleware/middleware.h"
#include "hardware/watchdog.h"
#include "pico/bootrom.h"
#include "FreeRtosQueueWrapper.h"

static const uint8_t sck_pin = 2;
static const uint8_t miso_pin = 0;
static const uint8_t mosi_pin = 3;
static const uint8_t cs_pin = 1;
static const uint32_t baudrate = 5000 * 1000;


void readDeviceID() {
    uint8_t id[6];
    flashReadId(id, 6);
    printf("Device ID is: ");
    printf("%02X%02X%02X%02X%02X", id[0], id[1], id[2], id[3], id[4]);
    printf("\n");
}
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

    setCommunication(getResponse);
    
    while (1) {
    
        char input = getchar_timeout_us(10000);
    
        switch (input) {
        case 'i':
            init_helper(spi, baudrate);
            readDeviceID();
            spiDeinit(spi, cs_pin, sck_pin, mosi_pin, miso_pin);
            break;
    
        case 'F':
            init_helper(spi, baudrate);
            printf("ack\n");
            configErrorCode_t configErrorCode=configure(0, 86116);
            if(configErrorCode==CONFIG_ERASE_ERROR){
                PRINT("ERASE ERROR")
            }
            PRINT("done")
            spiDeinit(spi, cs_pin, sck_pin, mosi_pin, miso_pin);
            break;
        case 'V':
            init_helper(spi, baudrate);
            printf("ack\n");
            fpgaConfigurationVerifyConfiguration();
            spiDeinit(spi, cs_pin, sck_pin, mosi_pin, miso_pin);
            break;
    
        case 'L':
            env5HwLedsAllOn();
            break;
        case 'l':
            env5HwLedsAllOff();
            break;
        case 'P':
            env5HwFpgaPowersOn();
            break;
        case 'p':
            env5HwFpgaPowersOff();
            break;
        case 'r':
            // env5HwFpgaFlashSpiDeinit();
            env5HwFpgaReset(1);
            sleep_ms(10);
            env5HwFpgaReset(0);
            break;
        case 'C':
            middleware_init();
            middleware_configure_fpga(0x0000);
            printf("reconfig 0x0000\r\n");
            middleware_deinit();
            break;
        case 'c':
            middleware_init();
            middleware_configure_fpga(0x00100000);
            printf("reconfig to 0x00100000\r\n");
            sleep_ms(1000);
            middleware_deinit();
            break;
        default:
            break;
        }
    }
}


HttpResponse_t *getResponse(uint32_t block_number) {
    // todo: deep copy response and return this instead of the response pointer

    HttpResponse_t *response;
    char baseUrl[] = "http://192.168.203.51:5000/getfile/%u";
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
