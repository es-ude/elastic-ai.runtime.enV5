//
// Created by David P. Federl
//

#ifndef ENV5_PAC193X_INTERNAL_HEADER
#define ENV5_PAC193X_INTERNAL_HEADER

#include "typedefs.h"
#include <stdint.h>

/* Datasheet:
 * https://ww1.microchip.com/downloads/en/DeviceDoc/PAC1931-Family-Data-Sheet-DS20005850E.pdf
 */

/* region CONSTANTS */

/*! Denominator for unipolar voltage measurement: 2^{16} = 65536 */
static const float UNIPOLAR_VOLTAGE_DENOMINATOR = ( float ) ( 1U << 16 );

/*! Denominator for unipolar power measurement: 2^{32} = 4294967296
 *
 * \Information This denominator is 2^{28} according to the datasheet,
 *            however testing has shown that 2^{32} is actually correct
 */
static const float UNIPOLAR_POWER_DENOMINATOR = ( float ) ( 1ULL << 32 );

/*! Denominator for energy measurement: 2^28 = 268435456 */
static const float ENERGY_DENOMINATOR = ( float ) ( 1ULL << 28 );

/*! rate for samples per second#
 *
 * \Important Must be set using the ctrl register */
static const float SAMPLING_RATE = 1024;

static pac193x_sensorConfiguration sensorConfiguration;

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
static pac193x_errorCode refresh ( void );

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
static pac193x_errorCode refreshV ( void );


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
static pac193x_errorCode setMeasurementProperties ( pac193x_measurementProperties * properties, pac193x_valueToMeasure valueToMeasure );

/*! send configuration to the sensor
 *
 * @param registerToWrite[in] address of the register where the settings should be stored
 * @param settingsToWrite[in] byte to store as settings
 * @return                    return the error code (0 if everything passed)
 */
static pac193x_errorCode sendConfigurationToSensor ( pac193x_registerAddress registerToWrite, pac193x_settings settingsToWrite );

/*! send request for register to read to sensor
 *
 * @param registerToRead[in] address of register to read
 * @return                   return the error code (0 if everything passed)
 */
static pac193x_errorCode sendRequestToSensor ( pac193x_registerAddress registerToRead );

/*! receive data from sensor
 *
 * @param responseBuffer[out]      byte buffer where the received will be stored
 * @param sizeOfResponseBuffer[in] size of the buffer for the response
 * @return                         return the error code (0 if everything passed)
 */
static pac193x_errorCode receiveDataFromSensor ( uint8_t * responseBuffer, uint8_t sizeOfResponseBuffer );

/*! requests and receives data from the sensor\n
 *  combines: sendRequestToSensor(...) and receiveDataFromSensor(...)
 *
 * @param responseBuffer[out]
 * @param sizeOfResponseBuffer[in]
 * @param registerToRead[in]
 * @return
 */
static pac193x_errorCode getDataFromSensor ( uint8_t * responseBuffer, uint8_t sizeOfResponseBuffer, pac193x_registerAddress registerToRead );


static uint64_t transformResponseBufferToUInt64 ( const uint8_t * responseBuffer, uint8_t sizeOfResponseBuffer );

static float convertToFloat ( uint64_t input );

static float calculateVoltageOfSense ( uint64_t input, uint8_t channel );

static float calculateVoltageOfSource ( uint64_t input, uint8_t channel );

static float calculateCurrentOfSense ( uint64_t input, uint8_t channel );

static float calculateActualPower ( uint64_t input, uint8_t channel );

static float calculateEnergy ( uint64_t input, uint8_t channel );

/* endregion */

#endif /* ENV5_PAC193X_INTERNAL_HEADER */
