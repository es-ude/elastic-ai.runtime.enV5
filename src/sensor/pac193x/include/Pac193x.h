#ifndef ENV5_PAC193X_HEADER
#define ENV5_PAC193X_HEADER

#include "Pac193xTypedefs.h"
#include <stdbool.h>
#include <stdint.h>

//tag::prototypes[]
/* region GENERAL FUNCTIONS */

/*!
 * @brief power up the sensor by setting PWRDN Pin to HIGH
 * @param sensor[in] configuration for sensor to use
 * @return           returns the error code (0 if everything passed)
 */
pac193xErrorCode_t pac193xPowerUpSensor(pac193xSensorConfiguration_t sensor);

/*!
 * @brief power down the sensor by setting PWRDN Pin to LOW
 * @IMPORTANT After powered up again all settings will be set to default!
 * @param sensor[in] configuration for sensor to use
 * @return           returns the error code (0 if everything passed)
 */
pac193xErrorCode_t pac193xPowerDownSensor(pac193xSensorConfiguration_t sensor);

/*!
 * @brief initializes the power sensor
 * @IMPORTANT
 *    function has to be called before the sensor can be used \n
 *    needs max 1.5ms for idle state after power up \n
 *    function pac193xPowerUpSensor(...) has to be called beforehand
 * @param sensor[in] configuration for sensor to use
 * @return           returns the error code (0 if everything passed)
 */
pac193xErrorCode_t pac193xInit(pac193xSensorConfiguration_t sensor);

/*!
 * @brief updates the number of used channels
 * @param sensor[in] sensor configuration with updated channels
 * @return           returns the error code (0 if everything passed)
 */
pac193xErrorCode_t pac193xSetChannelsInUse(pac193xSensorConfiguration_t sensor);

/* endregion GENERAL FUNCTIONS */

/* region SINGLE SHOT MEASUREMENTS */

/*!
 * @brief retrieve the production information from the sensor
 * @param sensor[in] configuration of the sensor to use
 * @param info[out]  struct that holds the information read from the sensor
 * @return           returns the error code (0 if everything passed)
 */
pac193xErrorCode_t pac193xGetSensorInfo(pac193xSensorConfiguration_t sensor,
                                        pac193xSensorId_t *info);

/*!
 * @brief read a specific value from the sensor for a specific channel
 * @param sensor[in]         configuration of the sensor to use
 * @param channel[in]        channel where the measurement should be taken from
 * @param valueToMeasure[in] value to be measured
 * @param value[out]         memory where the retrieved value will be stored
 * @return                   returns the error code (0 if everything passed)
 */
pac193xErrorCode_t pac193xGetMeasurementForChannel(pac193xSensorConfiguration_t sensor,
                                                   pac193xChannel_t channel,
                                                   pac193xValueToMeasure_t valueToMeasure,
                                                   float *value);

/*!
 * @brief read \b all available single shot values from the sensor for a specific channel
 * @param sensor[in]        configuration of the sensor to use
 * @param channel[in]       channel where the measurement should be taken from
 * @param measurements[out] memory where the struct with the measured values will be stored
 * @return                  returns the error code (0 if everything passed)
 */
pac193xErrorCode_t pac193xGetAllMeasurementsForChannel(pac193xSensorConfiguration_t sensor,
                                                       pac193xChannel_t channel,
                                                       pac193xMeasurements_t *measurements);

/* endregion SINGLE SHOT MEASUREMENTS */

/* region CONTINUOUS MEASUREMENTS */

/*!
 * @brief start continuous measurement as accumulator/average with 1024 samples/second
 * @param sensor[in] configuration of the sensor to use
 * @return           returns the error code (o if everything passed)
 */
pac193xErrorCode_t pac193xStartAccumulation(pac193xSensorConfiguration_t sensor);

/*!
 * @brief stop continuous measurement and return to single shot mode
 * @param sensor[in] configuration of the sensor to use
 * @return           returns the error code (o if everything passed)
 */
pac193xErrorCode_t pac193XStopAccumulation(pac193xSensorConfiguration_t sensor);

/*!
 * @brief get the counter of accumulated values and the accumulated power values
 *        for all channels of the sensor
 * @param sensor[in]        configuration of the sensor to use
 * @param measurements[out] memory where the struct with the measured values will be stored
 * @return                  returns the error code (o if everything passed)
 */
pac193xErrorCode_t
pac193xReadAccumulatedPowerForAllChannels(pac193xSensorConfiguration_t sensor,
                                          pac193xPowerMeasurements_t *measurements);

/*!
 * @brief read an rolling average value of from sensor
 * @param sensor[in]         configuration of the sensor to use
 * @param channel[in]        channel where the measurement should be taken from
 * @param valueToMeasure[in] value to be measured
 * @param value[out]         memory where the retrieved value will be stored
 * @return                   returns the error code (o if everything passed)
 */
pac193xErrorCode_t pac193xReadAverageMeasurement(pac193xSensorConfiguration_t sensor,
                                                 pac193xChannel_t channel,
                                                 pac193xValueToMeasure_t valueToMeasure,
                                                 float *value);

/*!
 * @brief read an rolling average value of from sensor
 * @param sensor[in]        configuration of the sensor to use
 * @param channel[in]       channel where the measurement should be taken from
 * @param measurements[out] memory where the struct with the measured values will be stored
 * @return                  returns the error code (o if everything passed)
 */
pac193xErrorCode_t pac193xReadAllAverageMeasurementsForChannel(pac193xSensorConfiguration_t sensor,
                                                               pac193xChannel_t channel,
                                                               pac193xMeasurements_t *measurements);
/* endregion CONTINUOUS MEASUREMENTS */
//end::prototypes[]
#endif /* ENV5_PAC193X_HEADER */
