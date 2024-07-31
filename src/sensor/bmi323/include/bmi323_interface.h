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
int8_t bmi323Init(struct bmi3_dev *dev, spiConfiguration_t *spi);

/*!
 * @brief convert raw value to degree per second (DPS)
 *
 * @param[in] rawValue raw value to be converted
 * @param[in] range dps range for value conversion
 * @param[in] resolution bit width of the raw value
 *
 * @returns value in degree per second
 */
float bmi323LsbToDps(int16_t rawValue, uint8_t range, uint8_t resolution);

#endif // ENV5_BMI323_INTERFACE_HEADER
