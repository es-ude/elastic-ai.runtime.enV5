#ifndef ENV5_HARDWARE_CONTROLLER_HEADER
#define ENV5_HARDWARE_CONTROLLER_HEADER

#include <stdbool.h>

/*!
 * @brief Initialize LEDs and FPGA
 */
void env5HwControllerInit(void);

/*!
 * @brief Initialize GPIOs for LEDs
 */
void env5HwControllerLedsInit(void);
/*!
 * @brief Turn ON all LEDs
 */
void env5HwControllerLedsAllOff(void);
/*!
 * @brief Turn OFF all LEDs
 */
void env5HwControllerLedsAllOn(void);

/*!
 * @brief Initialize FPGA GPIOs and turn it OFF
 */
void env5HwControllerFpgaInit(void);
/*!
 * @brief Power ON FPGA
 */
void env5HwControllerFpgaPowersOn(void);
/*!
 * @brief Power OFF FPGA
 */
void env5HwControllerFpgaPowersOff(void);
/*!
 * @brief Enable/Disable Reset of the FPGA
 * @param enable[in] true=enable reset, false=disable reset
 */
void env5HwFpgaReset(bool enable);

#endif // ENV5_HARDWARE_CONTROLLER_HEADER
