#ifndef ENV5_HARDWARE_CONTROLLER_HEADER
#define ENV5_HARDWARE_CONTROLLER_HEADER

#include <stdbool.h>

#define GPIO_LED0 22
#define GPIO_LED1 24
#define GPIO_LED2 25

#define FPGA_VOL_REGULATOR_EN_PIN 23 //! HIGH -> on, LOW -> off
#define FPGA_MOS_EN_PIN 21           //! LOW -> on, HIGH -> off
#define FPGA_RESET_CTRL_PIN 12       //! LOW -> on, HIGH -> off
#define FPGA_BUSY_PIN 15

void env5HwInit();

void env5HwLedsInit();
void env5HwLedsAllOff();
void env5HwLedsAllOn();

void env5HwFpgaInit();
void env5HwFpgaPowersOn();
void env5HwFpgaPowersOff();
void env5HwFpgaReset(bool enable);

#endif // ENV5_HARDWARE_CONTROLLER_HEADER
