#include "FreeRtosQueueWrapper.h"
#include "FreeRtosTaskWrapper.h"
#include "env5/Env5Hw.h"
#include "flash/Flash.h"
#include "hardware/watchdog.h"
#include "pico/stdlib.h"
#include "spi/Spi.h"
#include <hardware/spi.h>
#include <malloc.h>
#include <pico/bootrom.h>
#include <pico/stdio.h>
#include <stdio.h>

static const uint8_t REG_DEVID = 0x00;
static const uint8_t sck_pin = 2;
static const uint8_t miso_pin = 0;
static const uint8_t mosi_pin = 3;
static const uint8_t cs_pin = 1;

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
        if (getchar_timeout_us(10) == 'r' || !stdio_usb_connected()) {
            reset_usb_boot(0, 0);
        }
        watchdog_update();
        freeRtosTaskWrapperTaskSleep(1000);
    }
}

void init_helper(spi_inst_t *spi, uint32_t baudrate) {
    spiInit(spi, baudrate, cs_pin, sck_pin, mosi_pin, miso_pin);
    flashInit(cs_pin, spi);
}

// 256Mb flash, 4kb + 64 kb sector
void readDeviceID() {
    uint8_t *id [6];
    flashReadId(id, 6);
    printf("Device ID is: ");
    printf("%02X%02X%02X%02X%02X", id[0], id[1], id[2], id[3], id[4]);
    printf("\n");
    
}
void writeSPI() {
    uint16_t page_length = 256;
    uint8_t data[page_length];
    for (uint16_t i = 0; i < page_length; i++) {
        data[i] = i;
    }
    for (uint32_t i = 0; i < 2000; i++) {
        flashWritePage(REG_DEVID + i * 256, data, page_length);
    }
    printf(" bytes written\n");
}

void eraseSPISector() {
    for (uint32_t i = 0; i < 9; i++) {
        printf("round %u\n", i);
        int erased_sector = flashEraseData(65536 * i);
        if (erased_sector == 0) {
            printf("erased sector == 0\n");
        } else {
            printf("erased sector != 0\n");
        }

        uint8_t data_read[256];
        for (uint32_t k = 0; k < 256; k = k + 4) {
            data_read[k] = 0xD;
            data_read[k + 1] = 0xE;
            data_read[k + 2] = 0xA;
            data_read[k + 3] = 0xD;
        }
        flashReadData(65535 * i, data_read, 256);
        for (uint32_t j = 0; j < 256; j++) {
            printf("%u", data_read[j]);
        }
    }
}
void readSPI() {
    uint16_t page_length = 256;
    uint8_t data_read[(page_length)];
    for (uint32_t i = 0; i < 2000; i++) {
        int page_read = flashReadData(i * page_length, data_read, (page_length));
        printf("%u", page_read);
    }
    printf(" bytes read \n");
    printf("\n");
}
void spiTask() {

    spi_inst_t *spi = spi0;
    init_helper(spi, 1000 * 1000);

    while (1) {
        char input = getchar_timeout_us(10000);

        switch (input) {
        case 'i':
            readDeviceID();
            break;
        case 'e':
            eraseSPISector();
            break;
        case 'w':
            writeSPI();
            break;
        case 'o':
            readSPI();
            break;
        default:
            break;
        }
    }
}
int main() {
    stdio_init_all();
    // initHardwareTest();
    // freeRtosTaskWrapperRegisterTask(enterBootModeTaskHardwareTest, "enterBootModeTask");
    // freeRtosTaskWrapperRegisterTask(spiTask, "spiTask");
    // freeRtosTaskWrapperStartScheduler();
    spiTask();
}