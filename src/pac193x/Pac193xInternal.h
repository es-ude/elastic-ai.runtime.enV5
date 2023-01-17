#ifndef ENV5_PAC193X_INTERNAL_HEADER
#define ENV5_PAC193X_INTERNAL_HEADER

#include "Pac193xTypedefs.h"
#include <stdbool.h>
#include <stdint.h>

/* region SENSOR COMMUNICATION */

static pac193xErrorCode_t pac193xInternalCheckSensorAvailable(pac193xSensorConfiguration_t sensor);

static pac193xErrorCode_t
pac193xInternalSetDefaultConfiguration(pac193xSensorConfiguration_t sensor);

/*! send configuration to the sensor
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

/*! triggers reload of configuration and freezes accumulator
 *
 * \Information
 *   1. reloads configuration\n
 *   2. store current values of accumulator until next execution of refresh()\n
 *   3. resets the accumulator
 *
 * @param sensor[in] sensor to refresh
 * @return           returns the error code (0 if everything passed)
 */
static pac193xErrorCode_t pac193xInternalRefresh(pac193xSensorConfiguration_t sensor);

/*! triggers reload of configuration and freezes accumulator
 *
 * \Information
 *   1. reloads configuration\n
 *   2. store current values of accumulator until next execution of refresh()\n
 *   3. \b NOT resetting the accumulator
 *
 * @param sensor[in] sensor to refresh
 * @return           returns the error code (0 if everything passed)
 */
static pac193xErrorCode_t pac193xInternalRefreshV(pac193xSensorConfiguration_t sensor);

/*! send request for register to read to sensor
 *
 * @param registerToRead[in] address of register to read
 * @return                   return the error code (0 if everything passed)
 */
static pac193xErrorCode_t
pac193xInternalSendRequestToSensor(pac193xSensorConfiguration_t sensor,
                                   pac193xRegisterAddress_t registerToRead);

/*! receive data from sensor
 *
 * @param responseBuffer[out]      byte buffer where the received will be stored
 * @param sizeOfResponseBuffer[in] size of the buffer for the response
 * @return                         return the error code (0 if everything
 * passed)
 */
static pac193xErrorCode_t pac193xInternalReceiveDataFromSensor(pac193xSensorConfiguration_t sensor,
                                                               uint8_t *responseBuffer,
                                                               uint8_t sizeOfResponseBuffer);

/*! requests and receives data from the sensor\n
 *  combines: pac193xInternalSendRequestToSensor(...) and pac193xInternalReceiveDataFromSensor(...)
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

/*! store adequate properties for the requested measurement
 *
 * \Information
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

static float pac193xInternalCalculateVoltageOfSense(uint64_t input,
                                                    __attribute__((unused)) float resistor);

static float pac193xInternalCalculateVoltageOfSource(uint64_t input,
                                                     __attribute__((unused)) float resistor);

static float pac193xInternalCalculateCurrentOfSense(uint64_t input, float resistor);

static float pac193xInternalCalculateActualPower(uint64_t input, float resistor);

static float pac193xInternalCalculateEnergy(uint64_t input, float resistor);

/* endregion DATA TRANSFORMATION */

#endif /* ENV5_PAC193X_INTERNAL_HEADER */
