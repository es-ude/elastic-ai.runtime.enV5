// This program only works with the logic design with middleware
#include "pico/stdlib.h"
#include <stdio.h>
// #include "hardware/spi.h"
#include <stdlib.h>
#include <stdbool.h>
#include "middleware/middleware.h"

#define LED0_PIN 22
#define LED1_PIN 24
#define LED2_PIN 25

#define FPGA_VOL_REGULATOR_EN_PIN 23 // 1 -> on, 0 -> off
#define FPGA_MOS_EN_PIN 21 // 1 -> off, 0 -> on
#define FPGA_RESET_CTRL_PIN 12

// release them when you configure the FPGA
// what configure means? configure means FPGA wants to
// read the bit file from flash.
#define SPI_FPGA_FLASH_CS 1
#define SPI_FPGA_FLASH_MISO 0
#define SPI_FPGA_FLASH_MOSI 3
#define SPI_FPGA_FLASH_SCK 2

// Functions declaration
void fpga_flash_spi_deinit();
void fpga_powers_init();
void fpga_powers_on();

void fpga_powers_off();
void fpga_reset(unsigned int reset_en);
void leds_init(void);
void leds_all_on();
void fpga_reset_init(void);
void leds_all_off();
void printbuf(uint8_t *buf, int16_t len);


uint8_t dat_sent[14];
uint8_t dat_recv[14];
int main()
{

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
    while(1)
    {
        char c = getchar_timeout_us(10000);
        if(c=='t')
        {
            printf("hello\r\n");
        }
        else if(c=='1')
        {

            middleware_set_leds(0xff);
            uint8_t read_data = middleware_get_leds();
            if (read_data==0x0f)
                printf("leds all on\r\n");
            else
                printf("set leds all on failed.\r\n");
        }
        else if(c=='2')
        {
            middleware_set_leds(0xf0);
            uint8_t read_data = middleware_get_leds();
            if (read_data==0x00)
                printf("leds all off\r\n");
            else
                printf("set leds all off failed.\r\n");
        }
        else if(c=='3')
        {
            middleware_configure_fpag(0x0000);
            printf("reconfig 0x0000\r\n");
        }
        else if(c=='4')
        {
            middleware_configure_fpag(0x00100000);
            printf("reconfig to 0x00100000\r\n");
            sleep_ms(1000);
            // try to read id and print it out
        }
        else if(c=='5')
        {
            uint8_t id = middleware_get_design_id();
            printf("design id: %02x\r\n", id);
            sleep_ms(1000);
            // try to read id and print it out
        }
        else if (c == 'L')
        {
            leds_all_on();
        }
        else if (c == 'l')
        {
            leds_all_off();
        }
        else if (c == 'F')
        {
            fpga_powers_on();
        }
        else if (c == 'f')
        {
            fpga_powers_off();
        }
        else if (c=='r')
        {
            fpga_reset(1);
            sleep_ms(100);
            fpga_reset(0);

        }
        else if (c =='w')
        {
            int err_cnt =0;
            for (int k=0; k<10; k++)
            {
                for(int i=0;i<14;i++)
                    dat_sent[i] = rand()%256;
                middleware_write_blocking(0x0000, dat_sent, 14);
                middleware_read_blocking(0x0000, dat_recv, 14);
                bool err_flag = false;
                for(int i=0;i<14;i++)
                {
                    if (dat_sent[i]!=dat_recv[i])
                    {
                        err_flag = true;
                        break;
                    }
                }
                if(err_flag)
                {
                    printf("Error in sequence %d, read doesn't match the write: \r\n Sent: ", k);
                    printbuf(dat_sent, 14);
                    printf("Recv: ");
                    printbuf(dat_recv, 14);
                    printf("\r\n");
                    err_cnt++;
                }
//                else
//                {
//                    printf("Sent: ", k);
//                    printbuf(dat_sent, 14);
//                    printf("Recv: ");
//                    printbuf(dat_recv, 14);
//                    printf("\r\n");
//                }
            }
            printf("SPI interface text finished, %d times error happened.\r\n", err_cnt);
        }

        else if (c>20 && c<128){
             printf("unknown char %c\r\n",c);
        }
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
    if(reset_en)
    {
        gpio_put(FPGA_RESET_CTRL_PIN, 0);
    }
    else
    {
        gpio_put(FPGA_RESET_CTRL_PIN, 1);
    }

}

void leds_all_on() {
    gpio_put(LED0_PIN, 1);
    gpio_put(LED1_PIN, 1);
    gpio_put(LED2_PIN, 1);
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

void fpga_reset_init(void) {
    gpio_init(FPGA_RESET_CTRL_PIN);
    gpio_set_dir(FPGA_RESET_CTRL_PIN, GPIO_OUT);
    gpio_put(FPGA_RESET_CTRL_PIN, 1);
}

void leds_all_off() {
    gpio_put(LED0_PIN, 0);
    gpio_put(LED1_PIN, 0);
    gpio_put(LED2_PIN, 0);
}

void printbuf(uint8_t *buf, int16_t len) {
    for (int i = 0; i < len; i++) {
        printf("%02x ", buf[i]);
    }
    printf("\r\n");
}