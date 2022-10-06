
#include "FreeRtosQueueWrapper.h"
#include "FreeRtosTaskWrapper.h"
#include "env5/Env5Hw.h"
#include "flash/Flash.h"
#include "fpgaConfig/FpgaConfiguration.h"
#include "hardware/watchdog.h"
#include "pico/stdlib.h"
#include "spi/Spi.h"
#include <hardware/spi.h>
#include <pico/bootrom.h>
#include <pico/stdio.h>
#include <stdio.h>

static const uint8_t sck_pin = 2;
static const uint8_t miso_pin = 0;
static const uint8_t mosi_pin = 3;
static const uint8_t cs_pin = 1;
static const uint32_t baudrate = 5000 * 1000;

void initHardwareTest(void) {
    // Did we crash last time -> reboot into boot rom mode
    if (watchdog_enable_caused_reboot()) {
        reset_usb_boot(0, 0);
    }
    stdio_init_all();
    while ((!stdio_usb_connected())) {}
    freeRtosQueueWrapperCreate();
    watchdog_enable(2000, 1);
}

void _Noreturn enterBootModeTaskHardwareTest(void) {
    while (true) {
        watchdog_update();
        freeRtosTaskWrapperTaskSleep(1000);
    }
}
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

void configTask() {
    spi_inst_t *spi = spi0;
    env5HwInit();

    while (1) {

        char input = getchar_timeout_us(10000);

        switch (input) {
        case 'i':
            init_helper(spi, baudrate);
            readDeviceID();
            spi_deinit(spi);
            break;
        case 'F':
            init_helper(spi, baudrate);
            printf("ack\n");
            fpgaConfigurationFlashConfiguration();
            spi_deinit(spi);
            break;
        case 'V':
            init_helper(spi, baudrate);
            printf("ack\n");
            fpgaConfigurationVerifyConfiguration();
            spi_deinit(spi);
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
            env5HwFpgaFlashSpiDeinit();
            env5HwFpgaReset(1);
            sleep_ms(10);
            env5HwFpgaReset(0);
            break;
        default:
            break;
        }
    }
}
int main() {
    initHardwareTest();
    freeRtosTaskWrapperRegisterTask(enterBootModeTaskHardwareTest, "enterBootModeTask");
    freeRtosTaskWrapperRegisterTask(configTask, "configTask");
    freeRtosTaskWrapperStartScheduler();
}