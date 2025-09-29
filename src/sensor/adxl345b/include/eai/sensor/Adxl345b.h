#ifndef ENV5_ADXL345B_PUBLIC_HEADER
#define ENV5_ADXL345B_PUBLIC_HEADER

/* TODO: Implement standby mode */

#include <stdint.h>

#include "Adxl345bTypedefs.h"

/*! To read the x-, y-, and z-Axis, there are four different modes available which can be set with
 * setFifoMode. Each axis has a resolution of 16 bits. Since I2C can only read one byte at a time,
 * we use uint8_t arrays with a size of 6 to store the raw data. Besides setting a FifoMode this
 * library offers varied options to change and use sensor configurations */

/*!
 * @brief initializes the sensor
 * @IMPORTANT
 *    - function has to be called before use of the sensor can be used \n
 *    needs max 1.5ms for idle state after power up
 * @IMPORTANT We highly recommend using the "enV5_hw_configuration_rev_[x]" -library
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
 * @IMPORTANT - The expected default mode is BYPASS
 *
 * @param sensor[in] configuration for sensor to use
 * @param fifoMode FIFO Mode to be set
 * @param samplesForTrigger number of samples to store before trigger acts (max 32)
 * @return return the error code (0 if everything passed)
 *
 * @note In trigger mode undesirable operation may occur if the value of the samples bits is 0.
 * Placing the device into bypass mode clears FIFO.
 * max samplesForFIFOTrigger 32 -> you can read 33 Samples: fifo+(x/y/z)-register
 */
adxl345bErrorCode_t adxl345bSetFIFOMode(adxl345bSensorConfiguration_t sensor, uint8_t fifoMode,
                                        uint16_t samplesForTrigger);

/*!
 * @brief convert raw data into g-values of xAxis,yAxis and zAxis
 * @IMPORTANT We highly recommend using the "enV5_hw_configuration_rev_[x]" -library
 *
 * @param sensor[in] configuration for sensor to use
 * @param xAxis,yAxis,zAxis[out] actual G values from the sensor
 * @param responseBuffer[in] needs to be at least 6 bytes
 * @return return the error code (0 if everything passed)
 *
 */
adxl345bErrorCode_t adxl345bConvertDataXYZ(adxl345bSensorConfiguration_t sensor, float *xAxis,
                                           float *yAxis, float *zAxis,
                                           const adxl345bRawData_t rawSample);

/*!
 * @brief changes the measurement range of the sensor
 * @IMPORTANT We highly recommend using the "enV5_hw_configuration_rev_[x]" -library
 * @WARNING changes configuration to full resolution, clears justify bit
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
 * @brief read the raw data from the sensor in bypass-mode
 *
 * @IMPORTANT We highly recommend using the "enV5_hw_configuration_rev_[x]" -library
 *
 * @param sensor[in] configuration for sensor to use
 * @param rawSamples[out] pointer to adxl345bRawData_t in which raw data received from sensor will be stored
 *
 * @return return the error code (0 if everything passed)
 */
adxl345bErrorCode_t adxl345bGetSingleMeasurement(adxl345bSensorConfiguration_t sensor,
                                                 adxl345bRawData_t *rawSamples);

/*!
 * @brief reads requested number of raw data from the sensor in stream-, trigger- or fifo-mode
 *
 * @IMPORTANT We highly recommend using the "enV5_hw_configuration_rev_[x]" -library
 *
 * @param sensor[in] configuration for sensor to use
 * @param rawSamples[out] raw samples array received from the sensor
 * @param sizeOfRawSamples[in] number of required samples
 * @return return the error code (0 if everything passed)
 */

adxl345bErrorCode_t adxl345bGetMultipleMeasurements(adxl345bSensorConfiguration_t sensor,
                                                    adxl345bRawData_t *rawSamples,
                                                    uint32_t sizeOfRawSamples);

/*!
 * @brief reads raw data from the sensor. limited by microseconds
 *
 * @IMPORTANT   - We highly recommend using the "enV5_hw_configuration_rev_[x]" -library
 * @IMPORTANT   - Can be interrupted.caller needs to ensure free rtos
 *
 * @param sensor[in] configuration for sensor to use
 * @param durationInMicroseconds[in] measuring-time in micoseconds
 * @param rawSamples[out] raw data array received from sensor
 * @param sizeOfRawSamples[in] size of rawSample
 * @return return the error code (0 if everything passed)
 */

adxl345bErrorCode_t adxl345bGetMeasurementsForNMicroseconds(adxl345bSensorConfiguration_t sensor,
                                                            uint32_t durationInMicroseconds,
                                                            adxl345bRawData_t *rawSamples,
                                                            uint32_t sizeOfRawSamples);
/*!
 * @brief trigger the execution of the self-test procedure
 *
 *
 * @WARNING after testing the sensor is configured in low power mode at 12.5Hz, 2G range with full
 * resolution.
 * @WARNING major configuration changes in:
 * @WARNING     - ADXL345B_REGISTER_POWER_CONTROL
 * @WARNING     - ADXL345B_REGISTER_DATA_FORMAT
 * @WARNING     - ADXL345B_REGISTER_INTERRUPT_ENABLE
 *
 * @IMPORTANT We highly recommend using the "enV5_hw_configuration_rev_[x]" -library
 * @IMPORTANT function assumes that the sensor is not moved during the process
 *
 * @param sensor[in] configuration for sensor to use
 * @param avarageX,avarageY,avarageZ[out] avagare of G Values for further calibration
 *
 * @return                          return the error code (0 if self-test passed)
 */
adxl345bErrorCode_t adxl345bPerformSelfTest(adxl345bSensorConfiguration_t sensor, int *avarageX,
                                            int *avarageY, int *avarageZ);

adxl345bErrorCode_t adxl345bResetTrigger(adxl345bSensorConfiguration_t sensor,
                                         uint8_t fifoInformation);

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

/*!
 * @brief activates measurement mode
 * @IMPORTANT We highly recommend using the "enV5_hw_configuration_rev_[x]" -library
 * @WARNING clears link bit, disables all sleep functions and sets the frequency of readings in
 * sleep mode to 8Hz
 * @param sensor[in]
 * @return return the error code (0 if everything passed)
 */
adxl345bErrorCode_t adxl345bActivateMeasurementMode(adxl345bSensorConfiguration_t sensor);
/*!
 *
 * @param sensor[in]
 * @return return the error code (0 if everything passed)
 */
adxl345bErrorCode_t adxl345bSetFullResolutionMode(adxl345bSensorConfiguration_t sensor);
/*!
 *
 * @param sensor[in]
 * @return the error code (0 if everything passed)
 */
adxl345bErrorCode_t adxl345bSetTenBitMode(adxl345bSensorConfiguration_t sensor);

#endif /* ENV5_ADXL345B_PUBLIC_HEADER */
