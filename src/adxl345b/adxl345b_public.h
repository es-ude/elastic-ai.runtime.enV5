//
// Created by David P. Federl
//

#ifndef ENV5_ADXL345B_PUBLIC_HEADER
#define ENV5_ADXL345B_PUBLIC_HEADER

/* TODO: Enable change of settings */
/* TODO: Implement standby mode */
/* TODO: Enable full resolution for data */
/* TODO: Implement Offset calibration via self test */

#include "typedefs.h"
#include <stdint.h>


/*! initializes the sensor\n
 *  function has to be called before use of the sensor can be used \n
 *  \b IMPORTANT: needs max 1.5ms for idle state after power up
 *
 * @param i2cHost[in] i2c line to be used with sensor
 * @return            return the error code (0 if everything passed)
 */
adxl345b_errorCode adxl345b_init ( i2c_inst_t * i2cHost );

/*! function to read the serial number from the sensor
 *
 * @param serialNumber[out] memory where the serial number received from the sensor is stored
 * @return                  return the error code (0 if everything passed)
 */
adxl345b_errorCode adxl345b_readSerialNumber ( uint8_t * serialNumber );

/*! function to read the actual measurements from the sensor
 *
 * @param xAxis,yAxis,zAxis[out] actual G values from the sensor
 * @return                       return the error code (0 if everything passed)
 */
adxl345b_errorCode adxl345b_readMeasurements ( float * xAxis, float * yAxis, float * zAxis );

/*! function to trigger the execution of the self test procedure
 *
 * @return             return the error code (0 if self test passed)
 */
adxl345b_errorCode adxl345b_performSelfTest ( );

#endif /* ENV5_ADXL345B_PUBLIC_HEADER */
