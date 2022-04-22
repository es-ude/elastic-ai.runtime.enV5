#include <stdio.h>
#include <pico/stdio.h>
#include <pico/bootrom.h>
#include <hardware/spi.h>
#include "spi/spi_handler.h"
#include "TaskWrapper.h"
#include "QueueWrapper.h"
#include "pico/stdlib.h"
#include "hardware/watchdog.h"

static const uint8_t REG_DEVID = 0x00;

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
        if (getchar_timeout_us(10) == 'r' || !stdio_usb_connected()) {
            reset_usb_boot(0, 0);
        }
        watchdog_update();
        TaskSleep(1000);
    }
}
void readDeviceID(){
    spi_inst_t *spi = spi0;
    spi_init_handler(spi, 1000 * 1000);
    uint8_t id[3];
    spi_read_data(spi, REG_DEVID, id, 3);
    printf("Device ID is: ");
    printf("%02X%02X%02X", id[0], id[1], id[2]);
    printf("\n");

    spi_deinit(spi);
}
void writeSPI (){
    spi_inst_t *spi = spi0;
    // Initialize SPI port at 1 MHz
    spi_init_handler(spi, 1000 * 1000);


    uint16_t page_length = 256;
    uint8_t data[page_length];
    for (uint16_t i = 0; i < page_length; i++) {
        data[i] = i;
    }
    int wrote_page = flash_write_page(spi, 0, data, page_length);
    printf("%u", wrote_page);
    printf(" bytes written\n");

    spi_deinit(spi);
    TaskSleep(1000);
}



void readSPI(){
    spi_inst_t *spi = spi0;
    // Initialize SPI port at 1 MHz
    spi_init_handler(spi, 1000 * 1000);
    uint16_t page_length = 256;
    uint8_t data_read[page_length];
    TaskSleep(5000);
    int page_read = flash_read_data(spi, 0, data_read, page_length);
    printf("%u", page_read);
    printf(" bytes read \n");
    for (uint16_t i = 0; i < page_length; i++) {
        printf("%u", data_read[i]);
    }
    printf("\n");
    spi_deinit(spi);
}
void spiTask(){

    while(1){
        char input= getchar_timeout_us(10000);
        switch (input) {
            case 'i':
                readDeviceID();
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
    initHardwareTest();
    RegisterTask(enterBootModeTaskHardwareTest, "enterBootModeTask");
    RegisterTask(spiTask, "spiTask");
    StartScheduler();

}