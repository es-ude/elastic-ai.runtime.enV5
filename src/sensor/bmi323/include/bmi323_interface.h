#ifndef ENV5_BMI323_INTERFACE_HEADER
#define ENV5_BMI323_INTERFACE_HEADER

#include <stdint.h>

/*!
 *  @brief This function is to select the interface between SPI and I2C.
 *
 *  @param[in,out] dev Structure instance of bmi3_dev
 *  @param[in] spi SPI configuration for the sensor
 *
 *  @return Status of execution
 *  @retval 0 -> Success
 *  @retval else -> Failure Info
 */
int8_t bmi323InterfaceInit(struct bmi3_dev *dev, spiConfiguration_t *spi);

float bmi323LsbToDps(int16_t rawValue, uint8_t dps, uint8_t bitWidth);

#endif // ENV5_BMI323_INTERFACE_HEADER
