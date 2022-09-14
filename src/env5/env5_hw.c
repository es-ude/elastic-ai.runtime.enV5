#include "env5_hw.h"
#include "pico/stdlib.h"


void fpga_flash_spi_deinit()
{
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
void fpga_powers_init()
{
    // voltage regulator off
    gpio_init(FPGA_VOL_REGULATOR_EN_PIN);
    gpio_set_dir(FPGA_VOL_REGULATOR_EN_PIN, GPIO_OUT);
    gpio_put(FPGA_VOL_REGULATOR_EN_PIN, 0);

    // MOS FETS off
    gpio_init(FPGA_MOS_EN_PIN);
    gpio_set_dir(FPGA_MOS_EN_PIN, GPIO_OUT);
    gpio_put(FPGA_MOS_EN_PIN, 1);
}


void fpga_powers_on()
{
    // voltage regulator on
    gpio_put(FPGA_VOL_REGULATOR_EN_PIN, 1);

    sleep_ms(10);

    // MOS FETS on
    gpio_put(FPGA_MOS_EN_PIN, 0);
}

void fpga_powers_off()
{
    // voltage regulator on
    gpio_put(FPGA_VOL_REGULATOR_EN_PIN, 0);

    // MOS FETS on
    gpio_put(FPGA_MOS_EN_PIN, 1);
}

void fpga_reset_init(void)
{
    gpio_init(FPGA_RESET_CTRL_PIN);
    gpio_set_dir(FPGA_RESET_CTRL_PIN, GPIO_OUT);
    gpio_put(FPGA_RESET_CTRL_PIN, 1);
}

void fpga_reset(unsigned int reset_en)
{
    if(reset_en)
    {
        gpio_put(FPGA_RESET_CTRL_PIN, 0);
    }
    else
    {
        gpio_put(FPGA_RESET_CTRL_PIN, 1);
    }

}





void leds_init(void)
{
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


void leds_all_on()
{
    gpio_put(LED0_PIN, 1);
    gpio_put(LED1_PIN, 1);
    gpio_put(LED2_PIN, 1);
}

void leds_all_off()
{
    gpio_put(LED0_PIN, 0);
    gpio_put(LED1_PIN, 0);
    gpio_put(LED2_PIN, 0);
}


void env5_init() {

//fpga_flash_spi_deinit();

fpga_reset_init();

leds_init();

fpga_powers_init();

fpga_reset(0);

// Enable UART so we can print status output
//  stdio_init_all();
}




