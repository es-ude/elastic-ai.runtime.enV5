
#include "FreeRtosQueueWrapper.h"
#include "FreeRtosTaskWrapper.h"
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

void initHardwareTest(void) {
    // Did we crash last time -> reboot into boot rom mode
    if (watchdog_enable_caused_reboot()) {
        reset_usb_boot(0, 0);
    }
    stdio_init_all();
    while ((!stdio_usb_connected())) {}
    CreateQueue();
    watchdog_enable(2000, 1);
}

void _Noreturn enterBootModeTaskHardwareTest(void) {
    while (true) {
        watchdog_update();
        TaskSleep(1000);
    }
}

void init_helper(spi_inst_t *spi, uint32_t baudrate) {
    SPI_init(spi, baudrate, cs_pin, sck_pin, mosi_pin, miso_pin);
    init_flash(cs_pin, spi);
}

void configTask() {
    spi_inst_t *spi = spi0;
    init_helper(spi, 5000 * 1000);

    while (1) {
        char input = getchar_timeout_us(10000);

        switch (input) {
        case 'F':
            printf("ack\n");
            configurationFlash();
            break;
        case 'V':
            printf("ack\n");
            verifyConfigurationFlash();
            break;
        default:
            break;
        }
    }
}
int main() {
    initHardwareTest();
    RegisterTask(enterBootModeTaskHardwareTest, "enterBootModeTask");
    RegisterTask(configTask, "configTask");
    StartScheduler();
}