//
// Created by David P. Federl
//

#ifndef ENV5_ADXL345B_INTERNAL_HEADER
#define ENV5_ADXL345B_INTERNAL_HEADER

/*! \b Datasheet: https://www.analog.com/media/en/technical-documentation/data-sheets/ADXL345.pdf \n
 * \n
 * \b INTERRUPTS -> Not connected!\n
 * \b FIFO -> Can not be used effectively due to unconnected interrupt pins
 */

#include "typedefs.h"
#include <stdint.h>


/* region CONSTANTS */

static const adxl345b_rangeSettings MEASUREMENTS_RANGE_SETTINGS = ADXL345B_2G_RANGE_SETTING;
static const adxl345b_maxValue      MAX_VALUE                   = ADXL345B_2G_MAX_VALUE;
static const adxl345b_msbMask       MSB_MASK                    = 0b00000011;

/*! possible Values for scale factor depends on Range and resolution,\n
 * resolution is always 10 bit => only range is relevant:\n
 *   ->  2G range: 0.0043f \n
 *   ->  4G range: 0.0087f \n
 *   ->  8G range: 0.0175f \n
 *   -> 16G range: 0.0345f
 */
static const adxl345b_scaleFactor SCALE_FACTOR_FOR_RANGE = 0.0043f;

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

/*! function to send the configuration to the sensor
 *
 * @param registerToWrite[in] address of the register to write the configuration bit to
 * @param configuration[in]   configuration bit to write to the sensor
 * @return                    return the error code (0 if everything passed)
 */
static adxl345b_errorCode writeConfigurationToSensor ( adxl345b_register registerToWrite, uint8_t configuration );

/*! function to convert the raw value received from the sensor to a actual float value
 *
 * @param rawData[in] raw data received from the sensor
 * @param gValue[out] real world G value
 * @return            return the error code (0 if everything passed)
 */
static adxl345b_errorCode convertRawValueToGValue ( const uint8_t rawData[2], float * gValue );

/* endregion*/

#endif /* ENV5_ADXL345B_INTERNAL_HEADER */
