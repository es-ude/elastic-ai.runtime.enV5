#ifndef ENV5_PAC193X_HEADER
#define ENV5_PAC193X_HEADER

#include "Pac193xTypedefs.h"
#include <stdbool.h>
#include <stdint.h>

// tag::prototypes[]
/*
 * IMPORTANT:
 *   We highly recommend using the settings from the `enV5_hw_configuration`-library,
 *   to set up the sensor configuration struct!
 */

/* region GENERAL FUNCTIONS */

/*!
 * @brief power up the sensor by setting PWRDN Pin to HIGH
 *
 * @warning needs ~1.5ms to reach idle state after power up
 *
 *
 * @param sensor[in] configuration for sensor to use
 */
void pac193xPowerUpSensor(const pac193xSensorConfiguration_t *sensor);

/*!
 * @brief power down the sensor by setting PWRDN Pin to LOW
 *
 * @important After powered up again all settings will be set to default!
 *
 * @param sensor[in] configuration for sensor to use
 */
void pac193xPowerDownSensor(const pac193xSensorConfiguration_t *sensor);

/*!
 * @brief initializes the power sensor
 *
 * @warning requires 1ms to reach idle state
 *
 * @important \a pac193xPowerUpSensor has to be called beforehand!
 *
 * @param sensor[in] configuration for sensor to use
 */
void pac193xInit(pac193xSensorConfiguration_t *sensor);

/*!
 * @brief updates the number of used channels
 *
 * @param sensor[in] sensor configuration with updated channels
 */
void pac193xSetChannelsInUse(const pac193xSensorConfiguration_t *sensor);

/*!
 * @brief start continuous measurement as accumulator/average with 1024 samples/second
 *
 * @param sensor[in]     configuration of the sensor to use
 */
void pac193xStartAccumulation(const pac193xSensorConfiguration_t *sensor);

/*!
 * @brief stop continuous measurement and return to single shot mode
 *
 * @param sensor[in] configuration of the sensor to use
 */
void pac193XStopAccumulation(const pac193xSensorConfiguration_t *sensor);

/*!
 * @brief set the sample rate for the sensor in accumulation mode
 *
 * @param sensor[in]     configuration of the sensor to use
 * @param sampleRate[in] sample rate to be used
 */
void pac193xSetSampleRate(pac193xSensorConfiguration_t *sensor, pac193xSampleRate_t sampleRate);

/*!
 * @brief this function transfers the latest accumulated values to the data register
 *        and \b resets all accumulating registers afterward.
 *
 * @important the values can be read from the data registers until another Refresh command is
 * issued!
 *
 * @warning sensor needs 1ms to reach idle state
 *
 * @param sensor[in] configuration if the sensor to use
 */
void pac193xRefreshDataAndResetAccumulator(const pac193xSensorConfiguration_t *sensor);

/*!
 * @brief this function transfers the latest accumulated values to the data register
 *        \b without resetting the accumulating registers.
 *
 * @important the values can be read from the data registers until another Refresh command is
 * issued!
 *
 * @warning sensor needs 1ms to reach idle state
 *
 * @param sensor[in] configuration if the sensor to use
 */
void pac193xRefreshData(const pac193xSensorConfiguration_t *sensor);

/* endregion GENERAL FUNCTIONS */

/*!
 * @brief retrieve the production information from the sensor
 *
 * @param sensor[in]  configuration of the sensor to use
 * @returns           struct containing information read from the sensor
 */
pac193xSensorId_t pac193xGetSensorInfo(const pac193xSensorConfiguration_t *sensor);

/* region READ MEASUREMENTS */

/*!
 * @brief read a specific value from the sensor for a specific channel
 *
 * @important
 *   \a pac193xRefreshDataAndResetAccumulator or \a pac193xRefreshData has to be called beforehand!
 *
 * @param sensor[in]         configuration of the sensor to use
 * @param channel[in]        channel where the measurement should be taken from
 * @param valueToMeasure[in] value to be measured
 * @returns                  retrieved value (float)
 */
float pac193xGetMeasurementForChannel(const pac193xSensorConfiguration_t *sensor,
                                      pac193xChannel_t channel,
                                      pac193xValueToMeasure_t valueToMeasure);

/*!
 * @brief read \b all available single shot values from the sensor for a specific channel
 *
 * @param sensor[in]   configuration of the sensor to use
 * @param channel[in]  channel where the measurement should be taken from
 * @returns            struct containing measured values
 */
pac193xMeasurements_t pac193xGetMeasurementsForChannel(const pac193xSensorConfiguration_t *sensor,
                                                       pac193xChannel_t channel);

/*!
 * @brief read all rolling averages from sensor
 *
 * @important
 *   \a pac193xRefreshDataAndResetAccumulator or \a pac193xRefreshData has to be called beforehand!
 *
 * @param sensor[in]        configuration of the sensor to use
 * @param channel[in]       channel where the measurement should be taken from
 * @returns                 struct containing measured values
 */
pac193xMeasurements_t pac193xGetAveragesForChannel(const pac193xSensorConfiguration_t *sensor,
                                                   pac193xChannel_t channel);

/*!
 * @brief get the counter of accumulated values and the accumulated power values
 *        for all channels of the sensor
 *
 * @important
 *   \a pac193xRefreshDataAndResetAccumulator or \a pac193xRefreshData has to be called beforehand!
 *
 * @param sensor[in]   configuration of the sensor to use
 * @returns            struct containing measured values
 */
pac193xEnergyMeasurements_t
pac193xReadEnergyForAllChannels(const pac193xSensorConfiguration_t *sensor);

/* endregion ACCUMULATED MEASUREMENTS */
// end::prototypes[]

#endif /* ENV5_PAC193X_HEADER */
