#define SOURCE_FILE "ENV5-HW"

#include "Env5Hw.h"
#include "Common.h"
#include "pico/stdlib.h"

void env5HwFpgaFlashSpiDeinit() {
    gpio_init(SPI_FPGA_FLASH_CS);
    gpio_set_dir(SPI_FPGA_FLASH_CS, GPIO_IN);
    gpio_init(SPI_FPGA_FLASH_MISO);
    gpio_set_dir(SPI_FPGA_FLASH_MISO, GPIO_IN);
    gpio_init(SPI_FPGA_FLASH_MOSI);
    gpio_set_dir(SPI_FPGA_FLASH_MOSI, GPIO_IN);
    gpio_init(SPI_FPGA_FLASH_SCK);
    gpio_set_dir(SPI_FPGA_FLASH_SCK, GPIO_IN);
}

// PAC1934 is using pio6 and pio7 which is connected to I2C1
void env5HwFpgaPowerInit() {
    // voltage regulator off
    gpio_init(FPGA_VOL_REGULATOR_EN_PIN);
    gpio_set_dir(FPGA_VOL_REGULATOR_EN_PIN, GPIO_OUT);
    gpio_put(FPGA_VOL_REGULATOR_EN_PIN, 0);

    // MOS FETS off
    gpio_init(FPGA_MOS_EN_PIN);
    gpio_set_dir(FPGA_MOS_EN_PIN, GPIO_OUT);
    gpio_put(FPGA_MOS_EN_PIN, 1);
}

void env5HwFpgaPowersOn() {
    // voltage regulator on
    gpio_put(FPGA_VOL_REGULATOR_EN_PIN, 1);

    sleep_ms(10);

    // MOS FETS on
    gpio_put(FPGA_MOS_EN_PIN, 0);
    PRINT_DEBUG("FPGA Powered On.")
}

void env5HwFpgaPowersOff() {
    // voltage regulator on
    gpio_put(FPGA_VOL_REGULATOR_EN_PIN, 0);

    // MOS FETS on
    gpio_put(FPGA_MOS_EN_PIN, 1);
    PRINT_DEBUG("FPGA Powered Off.")
}

void env5HwFpgaResetInit(void) {
    gpio_init(FPGA_RESET_CTRL_PIN);
    gpio_set_dir(FPGA_RESET_CTRL_PIN, GPIO_OUT);
    gpio_put(FPGA_RESET_CTRL_PIN, 1);
}

void env5HwFpgaReset(unsigned int reset_en) {
    if (reset_en) {
        gpio_put(FPGA_RESET_CTRL_PIN, 0);
    } else {
        gpio_put(FPGA_RESET_CTRL_PIN, 1);
    }
}

void env5HwLedsInit(void) {
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

void env5HwLedsAllOn() {
    gpio_put(LED0_PIN, 1);
    gpio_put(LED1_PIN, 1);
    gpio_put(LED2_PIN, 1);
}

void env5HwLedsAllOff() {
    gpio_put(LED0_PIN, 0);
    gpio_put(LED1_PIN, 0);
    gpio_put(LED2_PIN, 0);
}

void env5HwInit() {
    env5HwFpgaResetInit();
    env5HwLedsInit();
    env5HwFpgaPowerInit();
    env5HwFpgaReset(0);
    env5HwLedsInit();
}
