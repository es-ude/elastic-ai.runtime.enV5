#ifndef ENV5_SENSORCONFIG_H
#define ENV5_SENSORCONFIG_H

#include "Adxl345bTypedefs.h"
#include "BusConfig.h"
#include "Pac193xTypedefs.h"
#include "Sht3xTypedefs.h"

#define ADXL_SLAVE ADXL345B_I2C_ALTERNATE_ADDRESS
#define ADXL_HOST I2C_INSTANCE
extern adxl345bSensorConfiguration_t adxl345bSensorConfiguration;

#define PAC_ONE_SLAVE PAC193X_I2C_ADDRESS_499R
#define PAC_ONE_HOST I2C_INSTANCE
#define PAC_ONE_POWER_PIN -1
#define PAC_ONE_R_SENSE                                                                            \
    { 0.82f, 0.82f, 0.82f, 0.82f }
#define PAC_ONE_USED_CHANNELS                                                                      \
    { .uint_channelsInUse = 0b00001111 }
extern pac193xSensorConfiguration_t pac193xSensorConfigurationOne;

#define PAC_TWO_SLAVE PAC193X_I2C_ADDRESS_806R
#define PAC_TWO_HOST I2C_INSTANCE
#define PAC_TWO_POWER_PIN -1
#define PAC_TWO_R_SENSE                                                                            \
    { 0.82f, 0.82f, 0.82f, 0.82f }
#define PAC_TWO_USED_CHANNELS                                                                      \
    { .uint_channelsInUse = 0b00001111 }
extern pac193xSensorConfiguration_t pac193xSensorConfigurationTwo;

#define SHT_SLAVE SHT3X_I2C_ADDRESS
#define SHT_HOST I2C_INSTANCE
extern sht3xSensorConfiguration_t sht3xSensorConfiguration;

#endif // ENV5_SENSORCONFIG_H
