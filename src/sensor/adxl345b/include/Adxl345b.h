#ifndef ENV5_ADXL345B_PUBLIC_HEADER
#define ENV5_ADXL345B_PUBLIC_HEADER

/* TODO: Implement standby mode */

#include <stdint.h>

#include "Adxl345bTypedefs.h"

/*! To read the x-, y-, and z-Axis, there are four different modes available which can be set with setFifoMode.
 * Each axis has a resolution of 16 bits. Since I2C can only read one byte at a time, we use uint8_t arrays with a size of 6 to store the raw data.
 * Besides setting a FifoMode this library offers varied options to change and use sensor configurations */

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
 * @IMPORTANT - We highly recommend using the "enV5_hw_configuration_rev_[x]" -library
 *            - The expected default mode is BYPASS
 *
 * @param sensor[in] configuration for sensor to use
 * @param fifoMode FIFO Mode to be set
 * @param samplesForTrigger number of samples to store before trigger acts (max 32)
 * @return return the error code (0 if everything passed)
 *
 * @note changing FIFOMode sets the number of samples in FIFO_CONTROL to 0.
 * Placing the device into bypass mode clears FIFO
 * max samplesForFIFOTrigger 32 -> you can read 33 Samples: fifo+(x/y/z)-register
 */
adxl345bErrorCode_t adxl345bSetFIFOMode(adxl345bSensorConfiguration_t sensor, uint8_t fifoMode, uint16_t samplesForTrigger);

/*!
 *@brief convert raw data into g-values of xAxis,yAxis and zAxis
 *
 *
 * @param xAxis,yAxis,zAxis[out] actual G values from the sensor
 * @param responseBuffer[in] needs to be at least 6 bytes
 * @return return the error code (0 if everything passed)
 *
 */
adxl345bErrorCode_t adxl345bConvertDataXYZ(float *xAxis, float *yAxis, float *zAxis, uint8_t *responseBuffer);


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
 * @brief read the raw data from the sensor
 *
 * @IMPORTANT We highly recommend using the "enV5_hw_configuration_rev_[x]" -library
 *
 * @param sensor[in] configuration for sensor to use
 * @param rawData[out] raw data received from the xAxis,yAxis,zAxis. Needs to be at least 6 bytes. (2 bytes each Axis)
 *
 * @return                       return the error code (0 if everything passed)
 */
adxl345bErrorCode_t adxl345bGetSingleMeasurement(adxl345bSensorConfiguration_t sensor, uint8_t* rawData);

/*!
 * @brief reads requested number of raw data from the sensor in stream, trigger or fifo mode
 *
 * @IMPORTANT We highly recommend using the "enV5_hw_configuration_rev_[x]" -library
 *
 * @param sensor[in] configuration for sensor to use
 * @param rawData[out] raw data array received from the xAxis,yAxis,zAxis. Each raw data needs to be at least 6 bytes. (2 bytes each Axis)
 * @param numberOfMeasurements[in] number of required Data
 * @return
 */
adxl345bErrorCode_t adxl345bGetMultipleMeasurements(adxl345bSensorConfiguration_t sensor, uint8_t **rawData, uint32_t numberOfMeasurements);

/*!
 * @brief reads raw data from the sensor in stream, trigger or fifo mode. limited by seconds or given buffer for rawData
 *
 * @IMPORTANT   - We highly recommend using the "enV5_hw_configuration_rev_[x]" -library
 *              - Can be interrupted.caller needs to ensure free rtos
 *
 * @param sensor[in] configuration for sensor to use
 * @param rawData[out] raw data array received from the xAxis,yAxis,zAxis. Each raw data needs to be at least 6 bytes. (2 bytes each Axis)
 * @param seconds[in] measuring-time in seconds
 * @param sizeOfRawData[in/out] maximum size of rawData which is manipulated to return actual size of read rawData
 * @return
 */
adxl345bErrorCode_t
adxl345bGetMeasurementsForNMilliseconds(adxl345bSensorConfiguration_t sensor, uint8_t *rawData, uint32_t seconds,
                                        uint32_t *sizeOfRawData);

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

adxl345bErrorCode_t adxl345bResetTrigger(adxl345bSensorConfiguration_t sensor, uint8_t fifoInformation);
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
