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

static const uint8_t REG_DEVID = 0x00;

void enterBootModeTask(void);

void init(void);
int reg_read(  spi_inst_t *spi,
               const uint cs,
               const uint8_t reg,
               uint8_t *buf,
               uint8_t nbytes);

int reg_read(  spi_inst_t *spi,
               const uint cs,
               const uint8_t reg,
               uint8_t *buf,
               const uint8_t nbytes) {

    int num_bytes_read = 0;
    uint8_t mb = 0;

    // Determine if multiple byte (MB) bit should be set
    if (nbytes < 1) {
        return -1;
    } else if (nbytes == 1) {
        mb = 0;
    } else {
        mb = 1;
    }

    // Construct message (set ~W bit high)
    uint8_t msg = 0x9F | (mb << 6) | reg;

    // Read from register
    gpio_put(cs, 0);
    spi_write_blocking(spi, &msg, 1);
    num_bytes_read = spi_read_blocking(spi, 0, buf, 3);
    gpio_put(cs, 1);

    return num_bytes_read;
}
void initSPI(){

}
void mainTask(void) {
    spi_inst_t *spi = spi0;

    const uint8_t cs_pin=1;
    const uint8_t miso_pin=0;
    const uint8_t mosi_pin=3;
    const uint8_t sclk_pin=2;
   // stdio_init_all();

    // Initialize CS pin high
    gpio_init(cs_pin);
    gpio_set_dir(cs_pin, GPIO_OUT);
    gpio_put(cs_pin, 1);

    // Initialize SPI port at 1 MHz
    spi_init(spi, 1000 * 1000);

    // Initialize SPI pins
    gpio_set_function(sclk_pin, GPIO_FUNC_SPI);
    gpio_set_function(mosi_pin, GPIO_FUNC_SPI);
    gpio_set_function(miso_pin, GPIO_FUNC_SPI);

    uint8_t data[3];
    reg_read(spi, cs_pin, REG_DEVID, data, 1);
    while(true){
        for (uint8_t i=0; i<3; i++){
            printf("%02X",data[i]);

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
