#ifndef ENV5_HARDWARE_CONTROLLER_HEADER
#define ENV5_HARDWARE_CONTROLLER_HEADER

#include <stdbool.h>

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
