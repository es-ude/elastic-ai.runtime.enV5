#define SOURCE_FILE "ENV5-HWC"

#include <stdbool.h>

#include "Common.h"
#include "EnV5HwConfiguration.h"
#include "EnV5HwController.h"
#include "Gpio.h"
#include "Sleep.h"

void env5HwControllerInit() {
    env5HwControllerLedsInit();
    env5HwControllerFpgaInit();
    env5HwControllerFpgaPowersOff();
}

/* region LED */

void env5HwControllerLedsInit(void) {
    gpioInitPin(LED0_GPIO, GPIO_OUTPUT);
    gpioSetPin(LED0_GPIO, GPIO_PIN_LOW);
    PRINT_DEBUG("LED0 initialized.");

    gpioInitPin(LED1_GPIO, GPIO_OUTPUT);
    gpioSetPin(LED1_GPIO, GPIO_PIN_LOW);
    PRINT_DEBUG("LED1 initialized.");

    gpioInitPin(LED2_GPIO, GPIO_OUTPUT);
    gpioSetPin(LED2_GPIO, GPIO_PIN_LOW);
    PRINT_DEBUG("LED2 initialized.");
}

void env5HwControllerLedsAllOn(void) {
    gpioSetPin(LED0_GPIO, GPIO_PIN_HIGH);
    gpioSetPin(LED1_GPIO, GPIO_PIN_HIGH);
    gpioSetPin(LED2_GPIO, GPIO_PIN_HIGH);
    PRINT_DEBUG("LED's enabled");
}

void env5HwControllerLedsAllOff(void) {
    gpioSetPin(LED0_GPIO, GPIO_PIN_LOW);
    gpioSetPin(LED1_GPIO, GPIO_PIN_LOW);
    gpioSetPin(LED2_GPIO, GPIO_PIN_LOW);
    PRINT_DEBUG("LED's disabled");
}

/* endregion LED */

/* region FPGA */

// PAC1934 is using pio6 and pio7 which is connected to I2C1
void env5HwControllerFpgaInit(void) {
    // setup FPGA reset control pin
    gpioInitPin(FPGA_RESET_CTRL_PIN, GPIO_OUTPUT);
    gpioSetPin(FPGA_RESET_CTRL_PIN, GPIO_PIN_HIGH);

    // turn the voltage regulator off => turn FPGA off
    gpioInitPin(FPGA_VOL_REGULATOR_EN_PIN, GPIO_OUTPUT);
    gpioSetPin(FPGA_VOL_REGULATOR_EN_PIN, GPIO_PIN_LOW);

    // turn the MOS-FETS off
    gpioInitPin(FPGA_MOS_EN_PIN, GPIO_OUTPUT);
    gpioSetPin(FPGA_MOS_EN_PIN, GPIO_PIN_LOW);

    // setup FPGA busy wait pin
    gpioInitPin(FPGA_BUSY_PIN, GPIO_INPUT);
}

void env5HwControllerFpgaPowersOn(void) {
    gpioSetPin(FPGA_VOL_REGULATOR_EN_PIN, GPIO_PIN_HIGH); // turn the voltage regulator on
    sleep_for_ms(10);                                     // wait until the FPGA is powered up
    gpioSetPin(FPGA_MOS_EN_PIN, GPIO_PIN_LOW);            // turn the MOS-FETS on
    sleep_for_ms(1);

    PRINT_DEBUG("FPGA Powered On.");
}

void env5HwControllerFpgaPowersOff(void) {
    gpioSetPin(FPGA_VOL_REGULATOR_EN_PIN, GPIO_PIN_LOW); // turn the voltage regulator off
    gpioSetPin(FPGA_MOS_EN_PIN, GPIO_PIN_HIGH);          // turn the MOS-FETS off
    sleep_for_ms(1);
    
    PRINT_DEBUG("FPGA Powered Off.");
}

void env5HwFpgaReset(bool enable) {
    gpioSetPin(FPGA_RESET_CTRL_PIN, enable ? GPIO_PIN_LOW : GPIO_PIN_HIGH);
}

/* endregion FPGA */
