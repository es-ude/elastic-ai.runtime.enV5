
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

#ifndef ENV5_ENV5_HW_H
#define ENV5_ENV5_HW_H

void leds_all_off();
void leds_all_on();
void fpga_reset(unsigned int reset_en);
void fpga_reset_init();
void fpga_powers_off();
void fpga_powers_on();
void fpga_flash_spi_deinit();
void fpga_reset_init();
void leds_init();
void fpga_powers_init();
void env5_init();

#endif // ENV5_ENV5_HW_H
