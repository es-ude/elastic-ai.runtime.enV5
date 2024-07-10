#ifndef ENV5_ADXL345B_PUBLIC_HEADER
#define ENV5_ADXL345B_PUBLIC_HEADER

/* TODO: Implement standby mode */

#include <stdint.h>

#include "Adxl345bTypedefs.h"


/*!
 * @brief initializes the sensor
 * @IMPORTANT
 *    - function has to be called before use of the sensor can be used \n
 *    needs max 1.5ms for idle state after power up
 * @IMPORTANT We highly recommend using the "enV5_hw_configuration_rev_[x]" -library
 *    -
 *
 * @param sensor[in] configuration for sensor to use
 *
 * @return            return the error code (0 if everything passed)
 */

adxl345bErrorCode_t adxl345bInit(adxl345bSensorConfiguration_t sensor);
/*!
 * @brief send the configuration to the sensor
 * @IMPORTANT We highly recommend using the "enV5_hw_configuration_rev_[x]" -library
 *
 * @param sensor[in] configuration for sensor to use
 * @param registerToWrite[in] address of the register to write the configuration bit to
 * @param configuration[in]   configuration bit to write to the sensor
 *
 * @return                    return the error code (0 if everything passed)
 * @note                       1600 & 3200 BW_RATE only available for SPI!
 */
adxl345bErrorCode_t adxl345bWriteConfigurationToSensor(adxl345bSensorConfiguration_t sensor,
                                                       adxl345bRegister_t registerToWrite,
                                                       adxl345bConfig_t config);

/*!
 * @brief sets FIFO Mode and interrupt information
 *
 * @IMPORTANT We highly recommend using the "enV5_hw_configuration_rev_[x]" -library
 *
 * @param sensor[in] configuration for sensor to use
 * @param fifoMode[in] selected mode to be activated
 * @param samplesForTrigger[in] number of required samples needed for trigger (max 32)
 * @return return the error code (0 if everything passed)
 *
 * @note changing FIFOMode sets the number of samples in FIFO_CONTROL to 0.
 * Placing the device into bypass mode clears FIFO
 * max samplesForTrigger 32 -> you can read 33 Samples: fifo+(x/y/z)-register
 */
adxl345bErrorCode_t adxl345bSetFIFOMode(adxl345bSensorConfiguration_t sensor, adxl345bFIFOMode_t fifoMode, uint8_t samplesForTrigger);

/*!
 *@brief convert raw data into g-values of xAxis,yAxis and zAxis
 *
 *
 * @param xAxis,yAxis,zAxis[out] actual G values from the sensor
 * @param responseBufferSizeSix[in]
 * @return return the error code (0 if everything passed)
 *
 */
adxl345bErrorCode_t adxl345bConvertDataXYZ(float *xAxis, float *yAxis, float *zAxis, uint8_t *responseBufferSizeSix);

/*!
 * @brief reads data of xAxis,yAxis and zAxis
 *
 * @IMPORTANT   - We highly recommend using the "enV5_hw_configuration_rev_[x]" -library
 *              - there must be at least 5 μs between the end of reading the data registers and the start of a new read
 *
 * @param sensor[in] configuration for sensor to use
 * @param responseBufferSizeSix[out] -> [6]: memory where data received from the sensor is stored
 * @return return the error code (0 if everything passed)
 *
 * @note read only, raw data needs to be converted into g-values
 */
adxl345bErrorCode_t adxl345bReadDataXYZ(adxl345bSensorConfiguration_t sensor, uint8_t *responseBufferSizeSix);


/*!
 * @brief change the measurement range of the sensor
 * @IMPORTANT We highly recommend using the "enV5_hw_configuration_rev_[x]" -library
 *
 * @param sensor[in] configuration for sensor to use
 * @param newRange[in] value of enum in Adxl345bTypedefs.h
 * @return             return the error code (0 if everything passed)
 */
adxl345bErrorCode_t adxl345bChangeMeasurementRange(adxl345bSensorConfiguration_t sensor,
                                                   adxl345bRange_t newRange);

/*!
 * @brief read the serial number from the sensor
 * @IMPORTANT We highly recommend using the "enV5_hw_configuration_rev_[x]" -library
 *
 * @param sensor[in] configuration for sensor to use
 * @param serialNumber[out] memory where the serial number received from the sensor is stored
 *
 * @return                  return the error code (0 if everything passed)
 */
adxl345bErrorCode_t adxl345bReadSerialNumber(adxl345bSensorConfiguration_t sensor,
                                             uint8_t *serialNumber);

/*!
 * @brief read the actual measurements from the sensor
 *
 * @IMPORTANT We highly recommend using the "enV5_hw_configuration_rev_[x]" -library
 *
 * @param sensor[in] configuration for sensor to use
 * @param rawDataSizeSix[out] raw data received from the xAxis,yAxis,zAxis. (2 bytes each Axis)
 *
 * @return                       return the error code (0 if everything passed)
 */
adxl345bErrorCode_t adxl345bReadMeasurementOneShot(adxl345bSensorConfiguration_t sensor, uint8_t* rawDataSizeSix);

/*!
 * @brief reads requested number of data in stream mode
 *
 * @IMPORTANT We highly recommend using the "enV5_hw_configuration_rev_[x]" -library
 *
 * @param sensor[in] configuration for sensor to use
 * @param rawData[out] raw data received from the xAxis,yAxis,zAxis. (2 bytes each Axis)
 * @param numberOfRequiredData[in] number of required Data
 * @return
 */
adxl345bErrorCode_t adxl345bReadMeasurementsInStream(adxl345bSensorConfiguration_t sensor, uint8_t (*rawData)[6], uint32_t numberOfRequiredData);
/*!
 * @brief trigger the execution of the self-test procedure
 *
 * function assumes that the sensor is not moved during the process
 * @IMPORTANT We highly recommend using the "enV5_hw_configuration_rev_[x]" -library
 *
 * @param sensor[in] configuration for sensor to use
 * @param deltaX,deltaY,deltaZ[out] delta of G Values
 *
 * @return                          return the error code (0 if self-test passed)
 */
adxl345bErrorCode_t adxl345bPerformSelfTest(adxl345bSensorConfiguration_t sensor, int *deltaX,
                                            int *deltaY, int *deltaZ);

/*!
 * @brief trigger a self calibration run
 *
 * function assumes that the X, Y error is zero at 0G and Z error is zero at 1G
 * @IMPORTANT We highly recommend using the "enV5_hw_configuration_rev_[x]" -library
 *
 * @param sensor[in] configuration for sensor to use
 * @return return the error code (0 if self-calibration passed)
 */
adxl345bErrorCode_t adxl345bRunSelfCalibration(adxl345bSensorConfiguration_t sensor);

#endif /* ENV5_ADXL345B_PUBLIC_HEADER */
