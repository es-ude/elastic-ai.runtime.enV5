//
// Created by David P. Federl
//

#ifndef ENV5_PAC193X_HEADER
#define ENV5_PAC193X_HEADER

/* Datasheet:
 * https://ww1.microchip.com/downloads/en/DeviceDoc/PAC1931-Family-Data-Sheet-DS20005850E.pdf
 */

#include "typedefs.h"
#include <stdint.h>

pac193x_errorCode pac193x_init(i2c_inst_t *i2cHost, float resistanceValues[4], uint8_t numberOfChannelsInUse);

void pac193x_setResistanceAtSense(float resistanceValues[4]);

pac193x_errorCode pac193x_setNumberOfChannelsInUse(uint8_t numberOfChannelsInUse);

pac193x_errorCode pac193x_getEnergyAndResetAccumulation(float energy[4]);

pac193x_errorCode pac193x_getSensorInfo(pac193x_info *info);

pac193x_errorCode pac193x_getMeasurementForChannel(pac193x_channel        channel,
                                                   pac193x_valueToMeasure valueToMeasure, float *value);

pac193x_errorCode pac193x_getAllMeasurementsForChannel(pac193x_channel       channel,
                                                       pac193x_measurements *measurements);

#endif /* ENV5_PAC193X_HEADER */
