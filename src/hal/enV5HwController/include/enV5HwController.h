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

/*!
 * @brief Initialize LEDs and FPGA
 */
void env5HwInit(void);

/*!
 * @brief Initialize GPIOs for LEDs
 */
void env5HwLedsInit(void);
/*!
 * @brief Turn ON all LEDs
 */
void env5HwLedsAllOff(void);
/*!
 * @brief Turn OFF all LEDs
 */
void env5HwLedsAllOn(void);

/*!
 * @brief Initialize FPGA GPIOs and turn it OFF
 */
void env5HwFpgaInit(void);
/*!
 * @brief Power ON FPGA
 */
void env5HwFpgaPowersOn(void);
/*!
 * @brief Power OFF FPGA
 */
void env5HwFpgaPowersOff(void);
/*!
 * @brief Enable/Disable Reset of the FPGA
 * @param enable true=enable reset, false=disable reset
 */
void env5HwFpgaReset(bool enable);

#endif // ENV5_HARDWARE_CONTROLLER_HEADER
