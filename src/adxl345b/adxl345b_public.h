//
// Created by David P. Federl
//

#ifndef ENV5_ADXL345B_PUBLIC_HEADER
#define ENV5_ADXL345B_PUBLIC_HEADER

/* TODO: Implement standby mode */

#include "typedefs.h"
#include <stdint.h>


/*! initializes the sensor\n
 *  function has to be called before use of the sensor can be used \n
 *  \b IMPORTANT: needs max 1.5ms for idle state after power up
 *
 * @param i2cHost[in] i2c line to be used with sensor
 * @param address[in] i2c slave address of the adxl345b (see typedefs.h)
 * @return            return the error code (0 if everything passed)
 */
adxl345b_errorCode adxl345b_init ( i2c_inst_t * i2cHost, adxl345b_i2c_slave_address i2cAddress );

/*! function to send the configuration to the sensor
 *
 * @param registerToWrite[in] address of the register to write the configuration bit to
 * @param configuration[in]   configuration bit to write to the sensor
 * @return                    return the error code (0 if everything passed)
 */
adxl345b_errorCode adxl345b_writeConfigurationToSensor ( adxl345b_register registerToWrite, adxl345b_configuration configuration );

/*! function to chagne the measurement range of the sensor
 *
 * @param newRange[in] value of \enum in typedefs.h
 * @return             return the error code (0 if everything passed)
 */
adxl345b_errorCode adxl345b_chagneMeasurementRange ( adxl345b_range newRange );

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

/*! function to trigger the execution of the self test procedure\n
 * \n
 * function assumes that the sensor is not moved during the process
 *
 * @return             return the error code (0 if self test passed)
 */
adxl345b_errorCode adxl345b_performSelfTest ( int * delta_x, int * delta_y, int * delta_z );

/*! function to trigger a slef calibration run\n
 * \n
 * function assumes that the X,Y error is zero at 0G and Z error is zero at 1G
 *
 * @return return the error code (0 if self test passed)
 */
adxl345b_errorCode adxl345b_runSelfCalibration ();

#endif /* ENV5_ADXL345B_PUBLIC_HEADER */
