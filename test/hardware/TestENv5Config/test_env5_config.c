//
// Created by Natalie Maman on 26.08.22.
//

#include <stdio.h>
#include <pico/stdio.h>
#include <pico/bootrom.h>
#include <hardware/spi.h>
#include "spi/spi_handler.h"
#include "flash/flash.h"
#include "TaskWrapper.h"
#include "QueueWrapper.h"
#include "pico/stdlib.h"
#include "hardware/watchdog.h"
#include "fpga_config/configuration.h"
#include "env5/env5_hw.h"


static const uint8_t sck_pin=2;
static const uint8_t  miso_pin=0;
static const uint8_t  mosi_pin=3;
static const uint8_t cs_pin=1;
static const uint32_t baudrate=5000*1000;

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
void readDeviceID(){
    uint8_t id [6];
    flash_read_id( id, 6);
    printf("Device ID is: ");
    printf("%02X%02X%02X%02X%02X", id[0], id[1], id[2],id[3], id[4]);
    printf("\n");

}
void init_helper(spi_inst_t *spi, uint32_t baudrate){
    SPI_init(spi, baudrate,cs_pin, sck_pin, mosi_pin, miso_pin);
    init_flash(cs_pin, spi);
}

void configTask(){
    spi_inst_t *spi = spi0;
    env5_init();


    while(1){

        char input= getchar_timeout_us(10000);

        switch (input) {
            case 'i':
                init_helper(spi, baudrate);
                readDeviceID();
                spi_deinit(spi);
                break;
            case 'F':
                init_helper(spi, baudrate);
                printf("ack\n");
                configurationFlash();
                spi_deinit(spi);
                break;
            case 'V':
                init_helper(spi, baudrate);
                printf("ack\n");
                verifyConfigurationFlash();
                spi_deinit(spi);
                break;
            case 'L':
                leds_all_on();
                break;
            case 'l':
                leds_all_off();
                break;
            case 'P':
                fpga_powers_on();
                break;
            case 'p':
                fpga_powers_off();
                break;
            case 'r':
                fpga_flash_spi_deinit();
                fpga_reset(1);
                sleep_ms(10);
                fpga_reset(0);
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