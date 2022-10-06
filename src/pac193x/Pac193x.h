#ifndef ENV5_PAC193X_HEADER
#define ENV5_PAC193X_HEADER

#include "Pac193xTypedefs.h"
#include <stdint.h>

/*! function that power up the sensor by setting PWRDN Pin to HIGH
 *
 * @return return the error code (0 if everything passed)
 */
pac193x_errorCode pac193x_powerUpSensor(void);

/*! function that power down the sensor by setting PWRDN Pin to LOW
 * \Important After powered up again all settings will be set to default!
 *
 * @return return the error code (0 if everything passed)
 */
pac193x_errorCode pac193x_powerDownSensor(void);

/*! initializes the power sensor
 *  \Important function has to be called before the sensor can be used \n
 *             needs max 1.5ms for idle state after power up
 *
 * @param i2cHost[in]               i2c line to be used with the sensor
 * @param resistanceValues[in]      array that holds the primary resistor values
 * for each used channel
 * @param numberOfChannelsInUse[in] number between 1 and 4 which specifies the
 * total number of used channels
 * @return                          return the error code (0 if everything
 * passed)
 */
pac193x_errorCode pac193x_init(i2c_inst_t *i2cHost, float resistanceValues[4],
                               pac193x_usedChannels usedChannels);

/*! updates the primary resistor values for each channel
 *  \Important values for \b all used channels must be specified!
 *
 * @param resistanceValues[in] array that holds the primary resistor values used
 * for each channel
 * @return                     return the error code (0 if everything passed)
 */
pac193x_errorCode pac193x_setResistanceAtSense(const float resistanceValues[4]);

/*! updates the number of used channels
 *
 * @param numberOfChannelsInUse[in] number between 1 and 4 which specifies the
 * total number of used channels
 * @return                          return the error code (0 if everything
 * passed)
 */
pac193x_errorCode pac193x_setChannelsInUse(pac193x_usedChannels usedChannels);

/*! function to retrieve the production information from the sensor
 *
 * @param info[out] struct that holds the information read from the sensor
 * @return          return the error code (0 if everything passed)
 */
pac193x_errorCode pac193x_getSensorInfo(pac193x_info *info);

/*! function to read a specific values from the sensor for a specific channel
 *
 * @param channel[in]        channel where the measurement should be taken from
 * @param valueToMeasure[in] value to be measured
 * @param value[out]         memory where the retrieved value will be stored
 * @return                   return the error code (0 if everything passed)
 */
pac193x_errorCode pac193x_getMeasurementForChannel(pac193x_channel channel,
                                                   pac193x_valueToMeasure valueToMeasure,
                                                   float *value);

/*! function to read \b all available values from the sensor for a specific
 * channel
 *
 * @param channel[in]       channel where the measurement should be taken from
 * @param measurements[out] memory where the struct with the measured values
 * will be stored
 * @return
 */
pac193x_errorCode pac193x_getAllMeasurementsForChannel(pac193x_channel channel,
                                                       pac193x_measurements *measurements);

#endif /* ENV5_PAC193X_HEADER */
