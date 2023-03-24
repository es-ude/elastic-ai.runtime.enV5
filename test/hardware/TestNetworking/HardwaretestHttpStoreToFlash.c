#define SOURCE_FILE "HTTP-FLASH-TEST"

#include "Common.h"
#include "Flash.h"
#include "FpgaConfigurationHttp.h"
#include "FreeRtosTaskWrapper.h"
#include "HTTP.h"
#include "HardwaretestHelper.h"
#include "Spi.h"
#include "env5/Env5Hw.h"
#include "middleware/middleware.h"
#include <hardware/spi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const uint8_t sck_pin = 2;
static const uint8_t miso_pin = 0;
static const uint8_t mosi_pin = 3;
static const uint8_t cs_pin = 1;
static const uint32_t baudrate = 5000 * 1000;
static char baseUrl[] = "http://192.168.178.24:5000/getfile/%u";

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

HttpResponse_t *getResponse(uint32_t block_number) {
    // todo: deep copy response and return this instead of the response pointer

    HttpResponse_t *response;
    char *URL = malloc(strlen(baseUrl) + block_number);
    sprintf(URL, baseUrl, block_number);

    uint8_t code = HTTPGet(URL, &response);
    PRINT_DEBUG("HTTP Get returns with %u", code);
    PRINT_DEBUG("Response Length: %li", response->length)

    free(URL);
    PRINT("done")
    return response;
}

void _Noreturn httpTask(void) {
    PRINT("=== STARTING TEST ===")
    connectToNetwork();
    spi_inst_t *spi = spi0;
    env5HwInit();

    setCommunication(getResponse);

    while (1) {

        char input = getchar_timeout_us(10000);

        switch (input) {
        case 'L':
            env5HwLedsAllOn();
            break;
        case 'l':
            env5HwLedsAllOff();
            break;
        case 'P':
            // power on FPGA
            // only required for old hardware before using flash
            env5HwFpgaPowersOn();
            break;
        case 'i':
            // test flash accessible
            init_helper(spi, baudrate);
            readDeviceID();
            spiDeinit(spi, cs_pin, sck_pin, mosi_pin, miso_pin);
            break;
        case 'F':
            // load bitfile to flash
            init_helper(spi, baudrate);
            printf("ack\n");
            configErrorCode_t configErrorCode = configure(0, 205272);
            if (configErrorCode == CONFIG_ERASE_ERROR) {
                PRINT("ERASE ERROR")
            }
            PRINT("done")
            spiDeinit(spi, cs_pin, sck_pin, mosi_pin, miso_pin);
            break;
        case 'V':
            // check bitfile on flash
            init_helper(spi, baudrate);
            printf("ack\n");
            fpgaConfigurationVerifyConfiguration();
            spiDeinit(spi, cs_pin, sck_pin, mosi_pin, miso_pin);
            break;
        case 'r':
            // reset FPGA
            env5HwFpgaReset(1);
            sleep_ms(10);
            env5HwFpgaReset(0);
            break;
        case 'C':
            // load bitfile from address 0
            middleware_init();
            middleware_configure_fpga(0x0000);
            printf("reconfig 0x0000\r\n");
            middleware_deinit();
            break;
        case 'c':
            // load bitfile from address 1
            middleware_init();
            middleware_configure_fpga(0x00100000);
            printf("reconfig to 0x00100000\r\n");
            sleep_ms(1000);
            middleware_deinit();
            break;
        case 'p':
            // power off FPGA
            env5HwFpgaPowersOff();
            break;

        case 'K':
            middleware_init();
            middleware_set_fpga_leds(0xff);
            uint8_t read_data = middleware_get_leds();
            if (read_data == 0x0f)
                printf("leds all on\r\n");
            else
                printf("set leds all on failed.\r\n");
            middleware_deinit();
            break;
        case 'k':
            middleware_init();
            middleware_set_fpga_leds(0xf0);
            if (middleware_get_leds() == 0x00)
                printf("leds all off\r\n");
            else
                printf("set leds all off failed.\r\n");
            middleware_deinit();
            break;

        default:
            break;
        }
    }
}

int main() {
    initHardwareTest();
    freeRtosTaskWrapperRegisterTask(enterBootModeTaskHardwareTest, "enterBootModeTask");
    freeRtosTaskWrapperRegisterTask(httpTask, "httpTask");
    freeRtosTaskWrapperStartScheduler();

    return 0;
}
