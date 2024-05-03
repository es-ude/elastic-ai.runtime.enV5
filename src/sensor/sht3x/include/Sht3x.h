#ifndef ENV5_SHT3X_HEADER
#define ENV5_SHT3X_HEADER

#include "Sht3xTypedefs.h"
#include <stdint.h>

// tag::prototypes[]
/*!
 * @brief initializes the temperature sensor
 * @IMPORTANT
 *    function has to be called before use of the sensor can be used \n
 *    needs max 1.5ms for idle state after power up
 * @param[in] i2cAddress i2c slave address of the sht3x (see typedefs.h)
 * @return            return the error code (0 if everything passed)
 */
sht3xErrorCode_t sht3xInit(sht3xSensorConfiguration_t sensor);

/*!
 * @brief read the value of the serial number from the sensor
 * @param[out] serialNumber memory where the serial number is stored
 * @return                  return the error code (0 if everything passed)
 */
sht3xErrorCode_t sht3xReadSerialNumber(sht3xSensorConfiguration_t sensor, uint32_t *serialNumber);

/*!
 * @brief read the status register (settings) from the sensor
 * @param[out] statusRegister memory where the status register is stored
 * @return                    return the error code (0 if everything passed)
 */
sht3xErrorCode_t sht3xReadStatusRegister(sht3xSensorConfiguration_t sensor, sht3xStatusRegister_t *statusRegister);

/*!
 * @brief read the temperature \b and the humidity from the sensor
 * @param[out] temperature memory where the temperature is stored
 * @param[out] humidity    memory where the temperature is stored
 * @return                 return the error code (0 if everything passed)
 */
sht3xErrorCode_t sht3xGetTemperatureAndHumidity(sht3xSensorConfiguration_t sensor, float *temperature, float *humidity);

/*!
 * @brief read \b only the temperature from the sensor
 * @param temperature[out] memory where the temperature is stored
 * @return                 return the error code (0 if everything passed)
 */
sht3xErrorCode_t sht3xGetTemperature(sht3xSensorConfiguration_t sensor, float *temperature);

/*!
 * @brief read \b only the humidity from the sensor
 * @IMPORTANT
 *    due to hardware limitations, the value of the temperature is read and processed
 * @param humidity[out] memory where the humidity is stored
 * @return              return the error code (0 if everything passed)
 */
sht3xErrorCode_t sht3xGetHumidity(sht3xSensorConfiguration_t sensor, float *humidity);

/*!
 * @brief get the last measured value from the sensor buffer
 * @param temperature[out] memory where the temperature is stored
 * @param humidity[out]    memory where the humidity is stored
 * @return                 return the error code (0 if everything passed)
 */
sht3xErrorCode_t sht3xReadMeasurementBuffer(sht3xSensorConfiguration_t sensor, float *temperature, float *humidity);

/*!
 * @brief enable the heater module of the sensor
 *
 * the heater can be used to check the plausibility of the measured values \n
 * the heater is automatically disabled after a reset
 *
 * @return return the error code (0 if everything passed)
 */
sht3xErrorCode_t sht3xEnableHeater(sht3xSensorConfiguration_t sensor);

/*!
 * @brief manually disable the heater module of the sensor
 * @return return the error code (0 if everything passed)
 */
sht3xErrorCode_t sht3xDisableHeater(sht3xSensorConfiguration_t sensor);

/*!
 * @brief trigger a soft reset of the sensor which recalibrates the sensor
 *        and resets the system controller
 * @IMPORTANT Hard RESET can be triggered by turning the power off and on again
 * @return return the error code (0 if everything passed)
 */
sht3xErrorCode_t sht3xSoftReset(sht3xSensorConfiguration_t sensor);
// end::prototypes[]
#endif /*ENV5_SHT3X_HEADER */
