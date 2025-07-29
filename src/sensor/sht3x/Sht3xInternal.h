#ifndef ENV5_SHT3X_INTERNAL_HEADER
#define ENV5_SHT3X_INTERNAL_HEADER

#include <stdint.h>

#include "eai/sensor/Sht3xTypedefs.h"

/* region CONSTANTS */

/*! constant to store the precalculated value for the CRC8 checksum */
extern const uint16_t sht3xCrc8Polynomial;

/*! constant to store the denominator used to calculate the temperature in degrees celsius */
extern const float sht3xDenominator;

/* endregion */

/* region FUNCTION PROTOTYPES */

/*!
 * @brief send a request to sensor via the i2c bus
 *
 * @param[in] command the 16 bit long command to be sent to the sensor
 *
 * @throws SHT3X_SEND_COMMAND_ERROR if data can't be sent
 * @throws SHT3X_INIT_ERROR if i2c not initialized
 */
static void sht3xInternalSendRequestToSensor(sht3xSensorConfiguration_t sensor,
                                             sht3xCommand_t command);

/*!
 * @brief function to receive data from the sensor via i2c bus
 *
 * @param responseBuffer[out]      memory where the received data is stored
 * @param sizeOfResponseBuffer[in] size of the response buffer
 *
 * @throws SHT3X_RECEIVE_DATA_ERROR if data can't be received
 * @throws SHT3X_INIT_ERROR if i2c not initialized
 */
static void sht3xInternalReceiveDataFromSensor(sht3xSensorConfiguration_t sensor,
                                               uint8_t *responseBuffer,
                                               uint8_t sizeOfResponseBuffer);

/*!
 * @brief function to perform a CRC8 checksum check on the received data
 *
 * calculates the 8 bit checksum of the first two bytes and compares it to the third byte of the
 * input
 *
 * @caution size has to be a multiple of 3
 *
 * @throws SHT3X_CHECKSUM_ERROR if checksum doesn't match
 */
static void sht3xInternalPerformChecksumCheck(const uint8_t *responseBuffer,
                                              uint8_t sizeOfResponseBuffer);

/*!
 * @brief function to calculate the CRC8 checksum of two given bytes
 *
 * based on the CRC8 algorithm from the datasheet
 *
 * @param[in] dataBuffer pointer to memory where two bytes are stored
 *
 * @returns checksum
 */
static uint8_t sht3xInternalCalculateChecksum(const uint8_t *dataBuffer);

/*!
 * @brief function to convert the raw data from the sensor to a real world
 * temperature value
 *
 * @returns temperature value in degree Celsius
 */
static float sht3xInternalCalculateTemperature(uint16_t rawValue);

/*!
 * @brief function to convert the raw data from the sensor to a real world humidity
 *        value
 *
 * @param[in] rawValue 16 bit raw data from the sensor
 *
 * @returns relative humidity in percent
 */
static float sht3xInternalCalculateHumidity(uint16_t rawValue);

/* endregion */

#endif /* ENV5_SHT3X_INTERNAL_HEADER */
