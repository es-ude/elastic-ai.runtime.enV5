#define SOURCE_FILE "HTTP-FLASH-TEST"

#include "CException.h"
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
#include <pico/stdlib.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static spi_inst_t *spi = spi0;
static const uint8_t sck_pin = 2;
static const uint8_t miso_pin = 0;
static const uint8_t mosi_pin = 3;
static const uint8_t cs_pin = 1;
static const uint32_t baudrate = 5000 * 1000;
char *baseUrl = NULL;
static char baseUrlSlow[] = "http://192.168.178.24:5000/getslow/%u";
static char baseUrlFast[] = "http://192.168.178.24:5000/getfast/%u";

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

void testFlashAccessible() {
    init_helper(spi, baudrate);
    readDeviceID();
    spiDeinit(spi, cs_pin, sck_pin, mosi_pin, miso_pin);
}

/// only available on special FPGA configurations!!
void getDesignId() {
    middleware_init();
    uint8_t fpga_design_id = middleware_get_design_id();
    printf("design id: %02x\r\n", fpga_design_id);
    sleep_ms(10);
    middleware_deinit();
}

void downloadBitfileToFlash(bool address0) {
    init_helper(spi, baudrate);
    configErrorCode_t configErrorCode0;
    if (address0) {
        baseUrl = baseUrlSlow;
        configErrorCode0 = configure(0x00000000, 85540); // blink slow
    } else {
        baseUrl = baseUrlFast;
        configErrorCode0 = configure(0x00100000, 86166); // blink fast
    }
    if (configErrorCode0 == CONFIG_ERASE_ERROR) {
        PRINT("ERASE ERROR")
    } else if (configErrorCode0 != CONFIG_NO_ERROR) {
        PRINT("ERROR")
    } else {
        PRINT("DONE")
    }
    spiDeinit(spi, cs_pin, sck_pin, mosi_pin, miso_pin);
}

void checkBitfileOnFlash() {
    init_helper(spi, baudrate);
    printf("ack\n");
    fpgaConfigurationVerifyConfiguration();
    spiDeinit(spi, cs_pin, sck_pin, mosi_pin, miso_pin);
}

void resetFPGA() {
    env5HwFpgaReset(1);
    sleep_ms(10);
    env5HwFpgaReset(0);
    PRINT("FPGA reset performed")
}

void loadBitfile(uint32_t address) {
    middleware_init();
    middleware_configure_fpga(address);
    PRINT("reconfig from 0x%08lX", address)
    sleep_ms(1000);
    middleware_deinit();
}

void setFpgaLed(bool on) {
    middleware_init();
    middleware_set_fpga_leds(on ? 0xFF : 0xF0);
    uint8_t read_data = middleware_get_leds();
    if (read_data == on ? 0x0F : 0x00) {
        PRINT("Set all LEDs")
    } else {
        PRINT("Set all LEDs failed")
    }
    middleware_deinit();
}

HttpResponse_t *getResponse(uint32_t block_number) {
    // todo: deep copy response and return this instead of the response pointer

    HttpResponse_t *response;
    char *URL = malloc(strlen(baseUrl) + block_number);
    sprintf(URL, baseUrl, block_number);

    CEXCEPTION_T exception;
    Try {
        HTTPGet(URL, &response);
        PRINT_DEBUG("HTTPGet Success! Response Length: %li", response->length);
    }
    Catch(exception) {}

    free(URL);
    PRINT("done")
    return response;
}

void _Noreturn httpTask(void) {
    PRINT("=== STARTING TEST ===")
    connectToNetwork();
    env5HwInit();
    setCommunication(getResponse);

    while (1) {
        char input = getchar_timeout_us(3000);
        switch (input) {
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
        case 'i':
            testFlashAccessible();
            break;
        case 'I':
            getDesignId();
            break;
        case 'f':
            downloadBitfileToFlash(true);
            break;
        case 'F':
            downloadBitfileToFlash(false);
            break;
        case 'V':
            checkBitfileOnFlash();
            break;
        case 'r':
            resetFPGA();
            break;
        case 'C':
            loadBitfile(0x00000000);
            break;
        case 'c':
            loadBitfile(0x00100000);
            break;
        case 'K':
            setFpgaLed(true);
            break;
        case 'k':
            setFpgaLed(false);
            break;
        default:
            break;
        }
    }
}

int main() {
    initHardwareTest();
    // freeRtosTaskWrapperRegisterTask(enterBootModeTaskHardwareTest, "enterBootModeTask");
    freeRtosTaskWrapperRegisterTask(httpTask, "httpTask", 0, FREERTOS_CORE_0);
    freeRtosTaskWrapperStartScheduler();

    return 0;
}
