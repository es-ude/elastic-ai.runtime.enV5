#ifndef ENV5_PAC193X_INTERNAL_HEADER
#define ENV5_PAC193X_INTERNAL_HEADER

#include "include/Pac193xTypedefs.h"
#include <stdbool.h>
#include <stdint.h>

typedef union pac193xInternalDataBuffer {
    uint64_t value;
    uint8_t raw[8];
} pac193xInternalDataBuffer_t;

/* region SENSOR COMMUNICATION */

static pac193xErrorCode_t pac193xInternalCheckSensorAvailable(pac193xSensorConfiguration_t sensor);

static pac193xErrorCode_t
pac193xInternalSetDefaultConfiguration(pac193xSensorConfiguration_t sensor);

/*!
 * @brief send configuration to the sensor
 *
 * @param registerToWrite[in] address of the register where the settings should
 * be stored
 * @param settingsToWrite[in] byte to store as settings
 * @return                    return the error code (0 if everything passed)
 */
static pac193xErrorCode_t
pac193xInternalSendConfigurationToSensor(pac193xSensorConfiguration_t sensor,
                                         pac193xRegisterAddress_t registerToWrite,
                                         pac193xSettings_t settingsToWrite);

/*!
 * @brief send request for register to read to sensor
 *
 * @param registerToRead[in] address of register to read
 * @return                   return the error code (0 if everything passed)
 */
static pac193xErrorCode_t
pac193xInternalSendRequestToSensor(pac193xSensorConfiguration_t sensor,
                                   pac193xRegisterAddress_t registerToRead);

/*!
 * @brief receive data from sensor
 *
 * @param responseBuffer[out]      byte buffer where the received will be stored
 * @param sizeOfResponseBuffer[in] size of the buffer for the response
 * @return                         return the error code (0 if everything
 * passed)
 */
static pac193xErrorCode_t pac193xInternalReceiveDataFromSensor(pac193xSensorConfiguration_t sensor,
                                                               uint8_t *responseBuffer,
                                                               uint8_t sizeOfResponseBuffer);

/*!
 * @brief requests and receives data from the sensor
 *
 * combines: \a pac193xInternalSendRequestToSensor and \a pac193xInternalReceiveDataFromSensor
 *
 * @param responseBuffer[out]
 * @param sizeOfResponseBuffer[in]
 * @param registerToRead[in]
 * @return
 */
static pac193xErrorCode_t pac193xInternalGetDataFromSensor(pac193xSensorConfiguration_t sensor,
                                                           uint8_t *responseBuffer,
                                                           uint8_t sizeOfResponseBuffer,
                                                           pac193xRegisterAddress_t registerToRead);

/* endregion SENSOR COMMUNICATION */

/* region SETUP MEASUREMENTS */

static bool pac193xInternalCheckChannelIsActive(pac193xUsedChannels_t usedChannels,
                                                pac193xChannel_t channelToTest);

static uint8_t pac193xInternalTranslateChannelToRSenseArrayIndex(pac193xChannel_t channel);

/*!
 * @brief store adequate properties for the requested measurement
 *
 * @information
 *   - register to start read\n
 *   - function to convert raw data\n
 *   - number of requested bytes
 *
 * @param properties[out]    memory where the properties should be stored
 * @param valueToMeasure[in] defines which value should be measured
 * @return                   return the error code (0 if everything passed)
 */
static pac193xErrorCode_t
pac193xInternalSetMeasurementProperties(pac193xMeasurementProperties_t *properties,
                                        pac193xValueToMeasure_t valueToMeasure);

static pac193xErrorCode_t pac193xInternalGetData(pac193xSensorConfiguration_t sensor,
                                                 pac193xChannel_t channel,
                                                 pac193xValueToMeasure_t valueToMeasure,
                                                 float *value);

/* endregion SETUP MEASUREMENTS */

/* region DATA TRANSFORMATION */

static uint64_t pac193xInternalTransformResponseBufferToUInt64(const uint8_t *responseBuffer,
                                                               uint8_t sizeOfResponseBuffer);

static float pac193xInternalConvertToFloat(uint64_t input);

static float pac193xInternalCalculateAccumulatorCount(uint64_t input,
                                                      __attribute((__unused__)) float resistor,
                                                      __attribute((__unused__)) uint8_t sampleRate);

static float pac193xInternalCalculateVoltageOfSense(uint64_t input,
                                                    __attribute((__unused__)) float resistor,
                                                    __attribute((__unused__)) uint8_t sampleRate);

static float pac193xInternalCalculateVoltageOfSource(uint64_t input,
                                                     __attribute((__unused__)) float resistor,
                                                     __attribute((__unused__)) uint8_t sampleRate);

static float pac193xInternalCalculateCurrentOfSense(uint64_t input, float resistor,
                                                    __attribute((__unused__)) uint8_t sampleRate);

static float pac193xInternalCalculatePower(uint64_t input, float resistor,
                                           __attribute((__unused__)) uint8_t sampleRate);

static float pac193xInternalCalculateEnergy(uint64_t input, float resistor, uint8_t sampleRate);

/* endregion DATA TRANSFORMATION */

#endif /* ENV5_PAC193X_INTERNAL_HEADER */
