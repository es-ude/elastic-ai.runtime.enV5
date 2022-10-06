#ifndef ENV5_PAC193X_INTERNAL_HEADER
#define ENV5_PAC193X_INTERNAL_HEADER

#include "Pac193xTypedefs.h"
#include <stdint.h>

/* Datasheet:
 * https://ww1.microchip.com/downloads/en/DeviceDoc/PAC1931-Family-Data-Sheet-DS20005850E.pdf
 */

/* region CONSTANTS */

/*! Denominator for unipolar voltage measurement: 2^{16} = 65536 */
static const float pac193xInternalUnipolarVoltageDenominator = (float)(1U << 16);

/*! Denominator for unipolar power measurement: 2^{32} = 4294967296
 *
 * \Information This denominator is 2^{28} according to the datasheet,
 *            however testing has shown that 2^{32} is actually correct
 */
static const float pac193xInternalUnipolarPowerDenominator = (float)(1ULL << 32);

/*! Denominator for energy measurement: 2^28 = 268435456 */
static const float pac193xInternalEnergyDenominator = (float)(1ULL << 28);

/*! rate for samples per second#
 *
 * \Important Must be set using the ctrl register */
static const float pac193xInternalSamplingRate = 1024;

/* endregion */

/* region FUNCTION PROTOTYPES */

/*! triggers reload of configuration and freezes accumulator
 *
 * \Information
 *   1. reloads configuration\n
 *   2. store current values of accumulator until next execution of refresh()\n
 *   3. resets the accumulator
 *
 * @return return the error code (0 if everything passed)
 */
static pac193xErrorCode_t pac193xInternalRefresh(void);

/*! triggers reload of configuration and freezes accumulator
 *
 * \Information
 *   1. reloads configuration\n
 *   2. store current values of accumulator until next execution of refresh()\n
 *   3. \b NOT resetting the accumulator
 *
 *
 * @return return the error code (0 if everything passed)
 */
static pac193xErrorCode_t pac193xInternalRefreshV(void);

/*! translate a passed Channel to the index of the RSense Array
 *
 * @param channel[in] Channel to use
 * @return            Index of the correct RSense value
 */
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

/*! send configuration to the sensor
 *
 * @param registerToWrite[in] address of the register where the settings should
 * be stored
 * @param settingsToWrite[in] byte to store as settings
 * @return                    return the error code (0 if everything passed)
 */
static pac193xErrorCode_t
pac193xInternalSendConfigurationToSensor(pac193xRegisterAddress_t registerToWrite,
                                         pac193xSettings_t settingsToWrite);

/*! send request for register to read to sensor
 *
 * @param registerToRead[in] address of register to read
 * @return                   return the error code (0 if everything passed)
 */
static pac193xErrorCode_t
pac193xInternalSendRequestToSensor(pac193xRegisterAddress_t registerToRead);

/*! receive data from sensor
 *
 * @param responseBuffer[out]      byte buffer where the received will be stored
 * @param sizeOfResponseBuffer[in] size of the buffer for the response
 * @return                         return the error code (0 if everything
 * passed)
 */
static pac193xErrorCode_t sht3xInternalReceiveDataFromSensor(uint8_t *responseBuffer,
                                                             uint8_t sizeOfResponseBuffer);

/*! requests and receives data from the sensor\n
 *  combines: pac193xInternalSendRequestToSensor(...) and pac193xInternalReceiveDataFromSensor(...)
 *
 * @param responseBuffer[out]
 * @param sizeOfResponseBuffer[in]
 * @param registerToRead[in]
 * @return
 */
static pac193xErrorCode_t pac193xInternalGetDataFromSensor(uint8_t *responseBuffer,
                                                           uint8_t sizeOfResponseBuffer,
                                                           pac193xRegisterAddress_t registerToRead);

static uint64_t pac193xInternalTransformResponseBufferToUInt64(const uint8_t *responseBuffer,
                                                               uint8_t sizeOfResponseBuffer);

static float pac193xInternalConvertToFloat(uint64_t input);

static float pac193xInternalCalculateVoltageOfSense(uint64_t input, uint8_t channel);

static float pac193xInternalCalculateVoltageOfSource(uint64_t input, uint8_t channel);

static float pac193xInternalCalculateCurrentOfSense(uint64_t input, uint8_t channel);

static float pac193xInternalCalculateActualPower(uint64_t input, uint8_t channel);

static float pac193xInternalCalculateEnergy(uint64_t input, uint8_t channel);

/* endregion */

#endif /* ENV5_PAC193X_INTERNAL_HEADER */
