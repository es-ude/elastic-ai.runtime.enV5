#define SOURCE_FILE "MAIN"
#define NDEBUG

#include <stdio.h>

#include "TaskWrapper.h"
#include "QueueWrapper.h"

//#include "Network.h"
#include "pico/bootrom.h"
#include "pico/stdlib.h"
#include "hardware/watchdog.h"
//#include "espMQTTBroker.h"
#include "hardware/spi.h"
#include "common.h"
#include "spi/spi.h"

static const uint8_t REG_DEVID = 0x00;

void enterBootModeTask(void);

void init(void);
//int reg_read(  spi_inst_t *spi,
//               const uint cs,
//               const uint8_t reg,
//               uint8_t *buf,
//               uint8_t nbytes);
//
//int reg_read(  spi_inst_t *spi,
//               const uint cs,
//               const uint8_t reg,
//               uint8_t *buf,
//               const uint8_t nbytes) {
//
//    int num_bytes_read = 0;
//    uint8_t mb = 0;

//    // Determine if multiple byte (MB) bit should be set
//    if (nbytes < 1) {
//        return -1;
//    } else if (nbytes == 1) {
//        mb = 0;
//    } else {
//        mb = 1;
//    }
//
//    // Construct message (set ~W bit high)
//    uint8_t msg = 0x9F | (mb << 6) | reg;
//
//    // Read from register
//    gpio_put(cs, 0);
//    spi_write_blocking(spi, &msg, 1);
//    num_bytes_read = spi_read_blocking(spi, 0, buf, 3);
//    gpio_put(cs, 1);
//
//    return num_bytes_read;
//}

void mainTask(void) {

    spi_inst_t *spi = spi0;

    // Initialize SPI port at 1 MHz
    spi_init_handler(spi, 1000 * 1000);


    uint8_t id[3];
    spi_read_data(spi,REG_DEVID, id, 3);

    uint16_t page_length=256;
    uint8_t data[page_length];
    for(uint16_t i=0; i<page_length;i++ ){
        data[i]=i;
    }
    int wrote_page= flash_write_page(spi, 0, data,page_length );
    printf("%u", wrote_page);
    uint8_t data_read[page_length];
    TaskSleep(10000);
    int page_read=flash_read_data(spi, 0,data_read, page_length);
    printf("%u", page_read);
    while(true){
        for (uint16_t i=0; i<3; i++){
            printf("%02X", id[i]);

        }
        printf("\n");
        printf("%u", wrote_page);
        printf("%u", page_read);
        printf("data read \n");
        for( uint16_t i=0; i<page_length; i++){
            printf("%u",data_read[i]);
        }
        printf("\n");
        TaskSleep(1000);
    }

}

int main() {
    init();

    RegisterTask(enterBootModeTask, "enterBootModeTask");
    RegisterTask((TaskCodeFunc) mainTask, "mainTask");
    StartScheduler();
}

void init(void) {
    // Did we crash last time -> reboot into bootrom mode
    if (watchdog_enable_caused_reboot()) {
        reset_usb_boot(0, 0);
    }
  //  Network_Init(false);
    // init usb, queue and watchdog
    stdio_init_all();
    while ((!stdio_usb_connected())) {}
    CreateQueue();
    watchdog_enable(2000, 1);
}

void _Noreturn enterBootModeTask(void) {
    while (true) {
        if (getchar_timeout_us(10) == 'r' || !stdio_usb_connected()) {
    //        ESP_MQTT_BROKER_Disconnect(true);
            PRINT("Enter boot mode...")
            reset_usb_boot(0, 0);
        }
        watchdog_update();
        TaskSleep(1000);
    }
}
