//
// Created by David P. Federl
//

#ifndef ENV5_SHT3X_HEADER
#define ENV5_SHT3X_HEADER

/* Datasheet:
 * https://www.sensirion.com/fileadmin/user_upload/customers/sensirion/Dokumente/2_Humidity_Sensors/Datasheets/Sensirion_Humidity_Sensors_SHT3x_Datasheet_digital.pdf
 */

#include "typedefs.h"
#include <stdint.h>

sht3x_errorCode sht3x_init(i2c_inst_t *i2cHost);

sht3x_errorCode sht3x_readSerialNumber(uint32_t *serialNumber);

sht3x_errorCode sht3x_readStatusRegister(sht3x_statusRegister *statusRegister);

sht3x_errorCode sht3x_getTemperatureAndHumidity(float *temperature, float *humidity);

sht3x_errorCode sht3x_getTemperature(float *temperature);

sht3x_errorCode sht3x_getHumidity(float *humidity);

sht3x_errorCode sht3x_readMeasurementBuffer(float *temperature, float *humidity);

sht3x_errorCode sht3x_enableHeater(void);

sht3x_errorCode sht3x_disableHeater(void);

sht3x_errorCode sht3x_softReset(void);


#endif /*ENV5_SHT3X_HEADER */
