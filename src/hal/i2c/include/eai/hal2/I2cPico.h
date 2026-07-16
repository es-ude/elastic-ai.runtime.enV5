#include "eai/hal2/I2c.h"
#include "hardware/i2c.h"

/*! \brief Handle for the I2C interface of Raspberry Pi Picos
 * \param pin_sda        GPIO num of used Serial Data Line (SDA)
 * \param pin_scl        GPIO num of used Serial Clock Line (SCL)
 * \param i2c_mod        I2C handler of Raspberry Pi Pico (i2c0 or i2c1)
 * \param fi2c_khz       Used I2C clock frequency [in kHz]
 */
struct I2cModule {
    uint8_t pin_sda;
    uint8_t pin_scl;
    i2c_inst_t *i2c_mod;
    uint16_t freq_khz;
    uint8_t status;
};
