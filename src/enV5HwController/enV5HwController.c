#define SOURCE_FILE "ENV5-HWC"

#include <stdbool.h>

#include "pico/stdlib.h"

#include "Common.h"
#include "Gpio.h"
#include "include/enV5HwController.h"

#define GPIO_LED0 22
#define GPIO_LED1 24
#define GPIO_LED2 25

#define FPGA_VOL_REGULATOR_EN_PIN 23 //! HIGH -> on, LOW -> off
#define FPGA_MOS_EN_PIN 21           //! LOW -> on, HIGH -> off
#define FPGA_RESET_CTRL_PIN 12       //! LOW -> on, HIGH -> off

void env5HwInit() {
    env5HwLedsInit();
    env5HwFpgaInit();
    env5HwFpgaReset(true);
}

/* region LED */

void env5HwLedsInit(void) {
    gpioInitPin(GPIO_LED0);
    gpioSetPin(GPIO_LED0, GPIO_PIN_LOW);
    PRINT_DEBUG("LED0 initialized.")

    gpioInitPin(GPIO_LED1);
    gpioSetPin(GPIO_LED1, GPIO_PIN_LOW);
    PRINT_DEBUG("LED1 initialized.")

    gpioInitPin(GPIO_LED2);
    gpioSetPin(GPIO_LED2, GPIO_PIN_LOW);
    PRINT_DEBUG("LED2 initialized.")
}

void env5HwLedsAllOn() {
    gpioSetPin(GPIO_LED0, GPIO_PIN_HIGH);
    gpioSetPin(GPIO_LED1, GPIO_PIN_HIGH);
    gpioSetPin(GPIO_LED2, GPIO_PIN_HIGH);
    PRINT_DEBUG("LED's enabled")
}

void env5HwLedsAllOff() {
    gpioSetPin(GPIO_LED0, GPIO_PIN_LOW);
    gpioSetPin(GPIO_LED1, GPIO_PIN_LOW);
    gpioSetPin(GPIO_LED2, GPIO_PIN_LOW);
    PRINT_DEBUG("LED's disabled")
}

/* endregion LED */

/* region FPGA */

// PAC1934 is using pio6 and pio7 which is connected to I2C1
void env5HwFpgaInit() {
    // setup FPGA reset control pin
    gpioInitPin(FPGA_RESET_CTRL_PIN);
    gpioSetPin(FPGA_RESET_CTRL_PIN, GPIO_PIN_HIGH);

    // turn the voltage regulator off
    gpioInitPin(FPGA_VOL_REGULATOR_EN_PIN);
    gpioSetPin(FPGA_VOL_REGULATOR_EN_PIN, GPIO_PIN_LOW);

    // turn the MOS-FETS off
    gpioInitPin(FPGA_MOS_EN_PIN);
    gpioSetPin(FPGA_MOS_EN_PIN, GPIO_PIN_LOW);
}

void env5HwFpgaPowersOn() {
    gpioSetPin(FPGA_VOL_REGULATOR_EN_PIN, GPIO_PIN_HIGH); // turn the voltage regulator on
    sleep_ms(10);                                         // wait until the FPGA is powered up
    gpioSetPin(FPGA_MOS_EN_PIN, GPIO_PIN_LOW);            // turn the MOS-FETS on

    PRINT_DEBUG("FPGA Powered On.")
}

void env5HwFpgaPowersOff() {
    gpioSetPin(FPGA_VOL_REGULATOR_EN_PIN, GPIO_PIN_LOW); // turn the voltage regulator off
    gpioSetPin(FPGA_MOS_EN_PIN, GPIO_PIN_HIGH);          // turn the MOS-FETS off

    PRINT_DEBUG("FPGA Powered Off.")
}

void env5HwFpgaReset(bool enable) {
    gpioSetPin(FPGA_RESET_CTRL_PIN, enable ? GPIO_PIN_LOW : GPIO_PIN_HIGH);
}

/* endregion FPGA */
