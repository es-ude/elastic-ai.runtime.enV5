// This program only works with the logic design with middleware
#include "pico/stdlib.h"
#include <stdio.h>
// #include "hardware/spi.h"
#include "middleware/middleware.h"
#include "echo_server.h"
#include "Common.h"

#include <stdbool.h>
#include <stdlib.h>

#define LED0_PIN 22
#define LED1_PIN 24
#define LED2_PIN 25

#define FPGA_VOL_REGULATOR_EN_PIN 23 // 1 -> on, 0 -> off
#define FPGA_MOS_EN_PIN 21           // 1 -> off, 0 -> on
#define FPGA_RESET_CTRL_PIN 12

// release them when you configure the FPGA
// what configure means? configure means FPGA wants to
// read the bit file from flash.
#define SPI_FPGA_FLASH_CS 1
#define SPI_FPGA_FLASH_MISO 0
#define SPI_FPGA_FLASH_MOSI 3
#define SPI_FPGA_FLASH_SCK 2

// Function declarations
void fpga_flash_spi_deinit();
void fpga_powers_init();
void fpga_powers_on();

void fpga_powers_off();
void fpga_reset(unsigned int reset_en);
void leds_init(void);
void leds_all_on();
void fpga_reset_init(void);
void leds_all_off();

int main() {

    fpga_flash_spi_deinit();
    fpga_reset_init();
    leds_init();
    fpga_powers_init();
    fpga_reset(0);
    sleep_ms(100);
    fpga_powers_on();
    sleep_ms(100);
    fpga_powers_off();

    // enable QXI interface to the FPGA
    middleware_init();

    stdio_init_all();

    // config FPGA
    bool deploy_successful = echo_server_deploy();
    if (!deploy_successful) {
        while(1) {
            PRINT("Deploy failed!")
            sleep_ms(3000);
        }
    }
    
    uint8_t counter = 0;

    // main loop
    while (1) {
        uint8_t return_val = (uint8_t) echo_server_echo(counter);

        if (return_val == counter+1) {
            leds_all_on();
            sleep_ms(500);
        }

        leds_all_off();
        sleep_ms(500);

        counter++;
    }
}

void fpga_flash_spi_deinit() {
    gpio_init(SPI_FPGA_FLASH_CS);
    gpio_set_dir(SPI_FPGA_FLASH_CS, GPIO_IN);
    gpio_init(SPI_FPGA_FLASH_MISO);
    gpio_set_dir(SPI_FPGA_FLASH_MISO, GPIO_IN);
    gpio_init(SPI_FPGA_FLASH_MOSI);
    gpio_set_dir(SPI_FPGA_FLASH_MOSI, GPIO_IN);
    gpio_init(SPI_FPGA_FLASH_SCK);
    gpio_set_dir(SPI_FPGA_FLASH_SCK, GPIO_IN);
}

void fpga_powers_init() {
    // voltage regulator off
    gpio_init(FPGA_VOL_REGULATOR_EN_PIN);
    gpio_set_dir(FPGA_VOL_REGULATOR_EN_PIN, GPIO_OUT);
    gpio_put(FPGA_VOL_REGULATOR_EN_PIN, 0);

    // MOS FETS off
    gpio_init(FPGA_MOS_EN_PIN);
    gpio_set_dir(FPGA_MOS_EN_PIN, GPIO_OUT);
    gpio_put(FPGA_MOS_EN_PIN, 1);
}

void fpga_powers_on() {
    // voltage regulator on
    gpio_put(FPGA_VOL_REGULATOR_EN_PIN, 1);

    sleep_ms(10);

    // MOS FETS on
    gpio_put(FPGA_MOS_EN_PIN, 0);
}

void fpga_powers_off() {
    // voltage regulator on
    gpio_put(FPGA_VOL_REGULATOR_EN_PIN, 0);

    // MOS FETS on
    gpio_put(FPGA_MOS_EN_PIN, 1);
}

void fpga_reset(unsigned int reset_en) {
    if (reset_en) {
        gpio_put(FPGA_RESET_CTRL_PIN, 0);
    } else {
        gpio_put(FPGA_RESET_CTRL_PIN, 1);
    }
}

void fpga_reset_init(void) {
    gpio_init(FPGA_RESET_CTRL_PIN);
    gpio_set_dir(FPGA_RESET_CTRL_PIN, GPIO_OUT);
    gpio_put(FPGA_RESET_CTRL_PIN, 1);
}

void leds_init(void) {
    gpio_init(LED0_PIN);
    gpio_set_dir(LED0_PIN, GPIO_OUT);
    gpio_put(LED0_PIN, 0);

    gpio_init(LED1_PIN);
    gpio_set_dir(LED1_PIN, GPIO_OUT);
    gpio_put(LED1_PIN, 0);

    gpio_init(LED2_PIN);
    gpio_set_dir(LED2_PIN, GPIO_OUT);
    gpio_put(LED2_PIN, 0);
}

void leds_all_on() {
    gpio_put(LED0_PIN, 1);
    gpio_put(LED1_PIN, 1);
    gpio_put(LED2_PIN, 1);
}

void leds_all_off() {
    gpio_put(LED0_PIN, 0);
    gpio_put(LED1_PIN, 0);
    gpio_put(LED2_PIN, 0);
}
