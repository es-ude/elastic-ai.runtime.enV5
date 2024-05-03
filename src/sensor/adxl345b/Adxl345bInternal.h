#ifndef ENV5_ADXL345B_INTERNAL_HEADER
#define ENV5_ADXL345B_INTERNAL_HEADER

/*! \b Datasheet: \n
 *      https://www.analog.com/media/en/technical-documentation/data-sheets/ADXL345.pdf
 * \n \b Quick \b Start \b Guide: \n
 *      https://www.analog.com/media/en/technical-documentation/application-notes/AN-1077.pdf
 * \n \n \b INTERRUPTS -> Not connected!\n \b FIFO -> Can not be used
 * effectively due to unconnected interrupt pins\n \b Sleep \b mode -> Can not
 * be used effectively due to unconnected interrupt pins\n \b Shock, Tap, Fall,
 * Activity \b detection -> Can not be used due to unconnected interrupt pins\n
 */

#include "Adxl345bTypedefs.h"
#include <stdint.h>

/* region FUNCTION PROTOTYPES */

/*! function to read the data from the sensor
 *
 * @param i2cConfiguration[in] i2c Configuration to be used with sensor
 * @param registerToRead[in]       address of the register to start the read
 * from
 * @param responseBuffer[out]      memory to store the received data
 * @param sizeOfResponseBuffer[in] size of the response buffer
 * @return                         return the error code (0 if everything
 * passed)
 */
static adxl345bErrorCode_t adxl345bInternalReadDataFromSensor(adxl345bSensorConfiguration_t sensor, adxl345bRegister_t registerToRead,
                                                              uint8_t *responseBuffer,
                                                              uint8_t sizeOfResponseBuffer);

/*! function to convert the raw value received from the sensor to a lsb value
 *
 * @param rawData[in]   raw data received from the sensor
 * @param lsbValue[out] LSB value
 * @return              return the error code (0 if everything passed)
 */
static adxl345bErrorCode_t adxl345bInternalConvertRawValueToLSB(const uint8_t rawData[2],
                                                                int *lsbValue);

/*! function to convert the lsb value received from the sensor to a actual float
 * value
 *
 * @param lsb[in] raw data received from the sensor
 * @param gValue[out] real world G value
 * @return            return the error code (0 if everything passed)
 */
static adxl345bErrorCode_t adxl345bInternalConvertLSBtoGValue(int lsb, float *gValue);

/*! combination of adxl345bInternalConvertRawValueToLSB(...) and
 * adxl345bInternalConvertLSBtoGValue(...)
 *
 * @param rawData[in] raw data received from the sensor
 * @param gValue[out] real world G value
 * @return            return the error code (0 if everything passed)
 */
static adxl345bErrorCode_t adxl345bInternalConvertRawValueToGValue(const uint8_t rawData[2],
                                                                   float *gValue);

/*! function to set the sensor into low power mode at 2G range with full
 * resolution
 *
 * @return return the error code (0 if everything passed)
 */
static adxl345bErrorCode_t adxl345bInternalWriteDefaultLowPowerConfiguration(adxl345bSensorConfiguration_t sensor);

/*! function to calculate the offset that should be passed to the sensor
 *
 * @param measuredDelta[in] delta measured during self test
 * @param maxValue[in]      max value that would be accepted
 * @param minValue[in]      min value that would be accepted
 * @return                  8 bit two complement that should be passed to the
 * sensor as offset
 */
static int8_t adxl345bInternalCalculateCalibrationOffset(int measuredDelta, int maxValue,
                                                         int minValue);

/* endregion*/

#endif /* ENV5_ADXL345B_INTERNAL_HEADER */
