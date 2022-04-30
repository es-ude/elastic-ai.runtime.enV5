//
// Created by David P. Federl
//

#ifndef ENV5_ADXL345B_INTERNAL_HEADER
#define ENV5_ADXL345B_INTERNAL_HEADER

/*! \b Datasheet: \n
 *      https://www.analog.com/media/en/technical-documentation/data-sheets/ADXL345.pdf \n
 *  \b Quick \b Start \b Guide: \n
 *      https://www.analog.com/media/en/technical-documentation/application-notes/AN-1077.pdf \n
 * \n
 * \b INTERRUPTS -> Not connected!\n
 * \b FIFO -> Can not be used effectively due to unconnected interrupt pins\n
 * \b Sleep \b mode -> Can not be used effectively due to unconnected interrupt pins\n
 * \b Shock, Tap, Fall, Activity \b detection -> Can not be used due to unconnected interrupt pins\n
 */

#include "typedefs.h"
#include <stdint.h>


/* region CONSTANTS */

static adxl345b_i2cSensorConfiguration i2c_SensorConfiguration; /*!< i2c configuration for the sensor */

static adxl345b_range         sensorMeasurementRange    = ADXL345B_2G_RANGE;
static adxl345b_rangeSettings measurementsRangeSettings = ADXL345B_2G_RANGE_SETTING;
static adxl345b_msbMask       msbMask                   = ADXL345B_2G_FULL_RESOLUTION;
static adxl345b_scaleFactor   scaleFactorForRange       = 0.0043f;

/* endregion */

/* region FUNCTION PROTOTYPES */

/*! function to read the data from the sensor
 *
 * @param registerToRead[in]       address of the register to start the read from
 * @param responseBuffer[out]      memory to store the received data
 * @param sizeOfResponseBuffer[in] size of the response buffer
 * @return                         return the error code (0 if everything passed)
 */
static adxl345b_errorCode readDataFromSensor ( adxl345b_register registerToRead, uint8_t * responseBuffer, uint8_t sizeOfResponseBuffer );

/*! function to convert the raw value received from the sensor to a lsb value
 *
 * @param rawData[in]   raw data received from the sensor
 * @param lsbValue[out] LSB value
 * @return              return the error code (0 if everything passed)
 */
static adxl345b_errorCode convertRawValueToLSB ( const uint8_t rawData[2], int * lsbValue );

/*! function to convert the lsb value received from the sensor to a actual float value
 *
 * @param lsb[in] raw data received from the sensor
 * @param gValue[out] real world G value
 * @return            return the error code (0 if everything passed)
 */
static adxl345b_errorCode convertLSBtoGValue ( int lsb, float * gValue );

/*! combination of convertRawValueToLSB(...) and convertLSBtoGValue(...)
 *
 * @param rawData[in] raw data received from the sensor
 * @param gValue[out] real world G value
 * @return            return the error code (0 if everything passed)
 */
static adxl345b_errorCode convertRawValueToGValue ( const uint8_t rawData[2], float * gValue );

/*! function to set the sensor into low power mode at 2G range with full resolution
 *
 * @return return the error code (0 if everything passed)
 */
static adxl345b_errorCode writeDefaultLowPowerConfiguration ( );

/* endregion*/

#endif /* ENV5_ADXL345B_INTERNAL_HEADER */
