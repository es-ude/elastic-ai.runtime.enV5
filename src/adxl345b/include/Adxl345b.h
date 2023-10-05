#ifndef ENV5_ADXL345B_PUBLIC_HEADER
#define ENV5_ADXL345B_PUBLIC_HEADER

/* TODO: Implement standby mode */

#include <stdint.h>

#include "Adxl345bTypedefs.h"

const static adxl345bRange_t ADXL345B_2G_RANGE = {2, 0b00001000, 0b00000011, 0.0043f};
const static adxl345bRange_t ADXL345B_4G_RANGE = {4, 0b00001001, 0b00000111, 0.0087f};
const static adxl345bRange_t ADXL345B_8G_RANGE = {8, 0b00001010, 0b00001111, 0.0175f};
const static adxl345bRange_t ADXL345B_16G_RANGE = {16, 0b00001011, 0b00111111, 0.0345f};

/*! initializes the sensor\n
 *  function has to be called before use of the sensor can be used \n
 *  \b IMPORTANT: needs max 1.5ms for idle state after power up
 *
 * @param i2cHost[in] i2c line to be used with sensor
 * @param address[in] i2c slave address of the adxl345b (see typedefs.h)
 * @return            return the error code (0 if everything passed)
 */
adxl345bErrorCode_t adxl345bInit(i2c_inst_t *i2cHost, adxl345bI2cSlaveAddress_t i2cAddress);

/*! function to send the configuration to the sensor
 *
 * @param registerToWrite[in] address of the register to write the configuration
 * bit to
 * @param configuration[in]   configuration bit to write to the sensor
 * @return                    return the error code (0 if everything passed)
 */
adxl345bErrorCode_t adxl345bWriteConfigurationToSensor(adxl345bRegister_t registerToWrite,
                                                       adxl345bConfiguration_t configuration);

/*! function to chagne the measurement range of the sensor
 *
 * @param newRange[in] value of \enum in typedefs.h
 * @return             return the error code (0 if everything passed)
 */
adxl345bErrorCode_t adxl345bChangeMeasurementRange(adxl345bRange_t newRange);

/*! function to read the serial number from the sensor
 *
 * @param serialNumber[out] memory where the serial number received from the
 * sensor is stored
 * @return                  return the error code (0 if everything passed)
 */
adxl345bErrorCode_t adxl345bReadSerialNumber(uint8_t *serialNumber);

/*! function to read the actual measurements from the sensor
 *
 * @param xAxis,yAxis,zAxis[out] actual G values from the sensor
 * @return                       return the error code (0 if everything passed)
 */
adxl345bErrorCode_t adxl345bReadMeasurements(float *xAxis, float *yAxis, float *zAxis);

/*! function to trigger the execution of the self test procedure\n
 * \n
 * function assumes that the sensor is not moved during the process
 *
 * @return             return the error code (0 if self test passed)
 */
adxl345bErrorCode_t adxl345bPerformSelfTest(int *deltaX, int *deltaY, int *deltaZ);

/*! function to trigger a slef calibration run\n
 * \n
 * function assumes that the X,Y error is zero at 0G and Z error is zero at 1G
 *
 * @return return the error code (0 if self test passed)
 */
adxl345bErrorCode_t adxl345bRunSelfCalibration();

#endif /* ENV5_ADXL345B_PUBLIC_HEADER */
