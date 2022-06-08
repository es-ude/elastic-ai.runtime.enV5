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

static const uint8_t REG_DEVID = 0x00;
static const uint8_t sck_pin=2;
static const uint8_t  miso_pin=0;
static const uint8_t  mosi_pin=3;
 static const uint8_t cs_pin=1;

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

void init_helper(spi_inst_t *spi, uint32_t baudrate){
    SPI_init(spi, baudrate,cs_pin, sck_pin, mosi_pin, miso_pin);
    init_flash(cs_pin, spi);
}

void readDeviceID(){
    uint8_t id[3];
    flash_read_id( id, 3);
    printf("Device ID is: ");
    printf("%02X%02X%02X", id[0], id[1], id[2]);
    printf("\n");

}
void writeSPI (){
    uint16_t page_length = 256;
    uint8_t data[page_length];
    for (uint16_t i = 0; i < page_length; i++) {
        data[i] = i;
    }
    int wrote_page = flash_write_page(REG_DEVID, data, page_length);
    printf("%u", wrote_page);
    printf(" bytes written\n");

}


void eraseSPISector(){
    int erased_sector= flash_erase_data( 0);
    if(erased_sector==1){
        printf("erased sector\n");
    }
}
void readSPI(){
    uint16_t page_length = 256;
    uint8_t data_read[page_length];
    int page_read = flash_read_data( REG_DEVID, data_read, page_length);
    printf("%u", page_read);
    printf(" bytes read \n");
    for (uint16_t i = 0; i < page_length; i++) {
        printf("%u", data_read[i]);
    }
    printf("\n");
}
void spiTask(){
    spi_inst_t *spi = spi0;
    init_helper(spi, 1000 * 1000);

    while(1){
        char input= getchar_timeout_us(10000);

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
    initHardwareTest();
    RegisterTask(enterBootModeTaskHardwareTest, "enterBootModeTask");
    RegisterTask(spiTask, "spiTask");
    StartScheduler();

}