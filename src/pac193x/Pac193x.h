#ifndef ENV5_PAC193X_HEADER
#define ENV5_PAC193X_HEADER

#include "Pac193xTypedefs.h"
#include <stdint.h>

/*! function that power up the sensor by setting PWRDN Pin to HIGH
 *
 * @param sensor[in] configuration for sensor to use
 * @return           returns the error code (0 if everything passed)
 */
pac193xErrorCode_t pac193xPowerUpSensor(pac193xSensorConfiguration_t sensor);

/*! function that power down the sensor by setting PWRDN Pin to LOW
 * \Important After powered up again all settings will be set to default!
 *
 * @param sensor[in] configuration for sensor to use
 * @return           returns the error code (0 if everything passed)
 */
pac193xErrorCode_t pac193xPowerDownSensor(pac193xSensorConfiguration_t sensor);

/*! initializes the power sensor
 *  \Important function has to be called before the sensor can be used \n
 *             needs max 1.5ms for idle state after power up
 *  \Important function pac193xPowerUpSensor(...) has to be called beforehand

 * @param sensor[in] configuration for sensor to use
 * @return           returns the error code (0 if everything passed)
 */
pac193xErrorCode_t pac193xInit(pac193xSensorConfiguration_t sensor);

/*! updates the number of used channels
 *
 * @param sensor[in] sensor configuration with updated channels
 * @return           returns the error code (0 if everything passed)
 */
pac193xErrorCode_t pac193xSetChannelsInUse(pac193xSensorConfiguration_t sensor);

/*! function to retrieve the production information from the sensor
 *
 * @param sensor[in] configuration of the sensor to use
 * @param info[out]  struct that holds the information read from the sensor
 * @return           returns the error code (0 if everything passed)
 */
pac193xErrorCode_t pac193xGetSensorInfo(pac193xSensorConfiguration_t sensor,
                                        pac193xSensorId_t *info);

/*! function to read a specific values from the sensor for a specific channel
 *
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

/*! function to read \b all available values from the sensor for a specific
 * channel
 *
 * @param sensor[in]        configuration of the sensor to use
 * @param channel[in]       channel where the measurement should be taken from
 * @param measurements[out] memory where the struct with the measured values will be stored
 * @return                  returns the error code (0 if everything passed)
 */
pac193xErrorCode_t pac193xGetAllMeasurementsForChannel(pac193xSensorConfiguration_t sensor,
                                                       pac193xChannel_t channel,
                                                       pac193xMeasurements_t *measurements);

#endif /* ENV5_PAC193X_HEADER */
