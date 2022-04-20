//
// Created by David P. Federl
//

#ifndef ENV5_SHT3X_INTERNAL_HEADER
#define ENV5_SHT3X_INTERNAL_HEADER

/*! Datasheet:
 * https://www.sensirion.com/fileadmin/user_upload/customers/sensirion/Dokumente/2_Humidity_Sensors/Datasheets/Sensirion_Humidity_Sensors_SHT3x_Datasheet_digital.pdf
 */

#include "typedefs.h"
#include <stdint.h>

/* region CONSTANTS */

/*! constant to store the precalculated value for the CRC8 checksum \n
 *  calculation P(x) = x^8 + x^5 + x^4 + 1 = 0b00110001= 0x31
 */
static const uint16_t CRC8_POLYNOMIAL = 0x31;

/*! constant to store the denominator used to calculate the temperature in degrees celsius \n
 *  calculation : 2^16 - 1
 */
static const float DENOMINATOR = ( 1 << 16 ) - 1.0f;

/* endregion */

/* region FUNCTION PROTOTYPES */

/*! send a request to sensor via the i2c bus
 *
 * @param command[in] the 16 bit long command to be send to the sensor
 * @return            returns the error code (0 if everything passed)
 */
static sht3x_errorCode sendRequestToSensor ( sht3x_command command );

/*! function to receive data from the sensor via i2c bus
 *
 * @param responseBuffer[out]      memory where the received data is stored
 * @param sizeOfResponseBuffer[in] size of the response buffer
 * @return                         returns the error code (0 if everything
 *                                 passed)
 */
static sht3x_errorCode receiveDataFromSensor ( uint8_t * responseBuffer, uint8_t sizeOfResponseBuffer );

/*! function to perform a CRC8 checksum check on the received data
 *  calculates the 8 bit checksum of the first two bytes and compares it to
 *  the third byte of the input \n
 *  \b CAUTION: size has to be a multiple of 3
 *
 * @param responseBuffer[in]       data to be tested
 * @param sizeOfResponseBuffer[in] size of the response buffer
 * @return                         returns the error code (0 if everything
 *                                 passed)
 */
static sht3x_errorCode performChecksumCheck ( const uint8_t * responseBuffer, uint8_t sizeOfResponseBuffer );

/*! function to calculate the CRC8 checksum of two given bytes
 *  based on the CRC8 algorithm from the datasheet
 *
 * @param dataBuffer[in] pointer to memory where two bytes are stored
 * @return               checksum of the input
 */
static uint8_t calculateChecksum ( const uint8_t * dataBuffer );

/*! function to convert the raw data from the sensor to a real world
 * temperature value
 *
 * @param rawValue[in] 16 bit raw data from the sensor
 * @return             returns the temperature in degree celcius
 */
static float calculateTemperature ( uint16_t rawValue );

/*! function to convert the raw data from the sensor to a real world humidity
 * value
 *
 * @param rawValue[in] 16 bit raw data from the sensor
 * @return             returns the humidity in RH\%
 */
static float calculateHumidity ( uint16_t rawValue );

/* endregion */

#endif // ENV5_SHT3X_INTERNAL_HEADER
