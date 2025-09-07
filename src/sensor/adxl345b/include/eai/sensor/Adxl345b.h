#ifndef ENV5_ADXL345B_PUBLIC_HEADER
#define ENV5_ADXL345B_PUBLIC_HEADER

#include <stdint.h>
#include <stdlib.h>

#include "Adxl345bTypedefs.h"

/*! To read the x-, y-, and z-Axis, there are four different modes available which can be set with
 * setFifoMode. Each axis has a resolution of 16 bits. Since I2C can only read one byte at a time,
 * we use uint8_t arrays with a size of 6 to store the raw data. Besides setting a FifoMode this
 * library offers varied options to change and use sensor configurations */

// #############################################################################
// Sensor Configuration
// #############################################################################

/*!
 * @brief initializes the sensor
 *
 * @attention function has to be called before use of the sensor can be used
 *            needs max 1.5ms for idle state after power up.
 *
 * @attention We highly recommend using the "enV5_hw_configuration_rev_[x]" -library for
 *            configuration.
 *
 * @note This function sets the sensor to default configuration:
 * @note   - Output Data Rate: 12.5Hz, Low Power Mode
 * @note   - Measurement Mode: Deactivated
 * @note   - Interrupts: All Disabled
 * @note   - Range: ±2G, 10bit mode
 *
 * @param sensor[inout]  configuration for sensor to use
 * @return               return the error code (0 if everything passed)
 */
adxl345bErrorCode_t adxl345bInit(adxl345bSensorConfiguration_t *sensor);

/*!
 * @brief read configuration from the sensor
 *
 * @param sensor[in]          sensor to use
 * @param targetRegister[in]  address of the register to write the configuration bit to
 * @param configuration[out]  buffer where the read configuration is stored
 * @return                    return the error code (0 if everything passed)
 */
adxl345bErrorCode_t adxl345bReadConfigurationFromSensor(adxl345bSensorConfiguration_t *sensor,
                                                        adxl345bRegister_t targetRegister,
                                                        uint8_t *config);

/*!
 * @brief update configuration at the sensor
 *
 * @attention This method overwrites the whole register at the sensor!
 *
 * @param sensor[in]          sensor to use
 * @param targetRegister[in]  address of the register to write the configuration bit to
 * @param configuration[in]   configuration byte to write
 * @return                    return the error code (0 if everything passed)
 */
adxl345bErrorCode_t adxl345bWriteConfigurationToSensor(adxl345bSensorConfiguration_t *sensor,
                                                       adxl345bRegister_t targetRegister,
                                                       adxl345bConfig_t config);

/*!
 * @brief sets FIFO Mode and threshold for internal buffer
 *
 * @param sensor[in]     sensor to use
 * @param fifoMode[in]   FIFO Mode to be used
 * @param threshold[in]  threshold for FIFO trigger (only lowest 5 bits are used):
 *                       - Bypass mode -> not used,
 *                       - FIFO mode -> number of samples until watermark interrupt is triggered,
 *                       - Stream mode -> number of samples until watermark interrupt is triggered,
 *                       - Trigger mode -> number of samples until trigger interrupt is triggered
 * @return               return the error code (0 if everything passed)
 */
adxl345bErrorCode_t adxl345bSetFIFOMode(adxl345bSensorConfiguration_t *sensor,
                                        adxl345bFifoMode_t fifoMode, uint8_t threshold);

/*!
 * @brief activates measurement mode
 *
 * @param sensor[in]  sensor to use
 * @return            return the error code (0 if everything passed)
 */
adxl345bErrorCode_t adxl345bActivateMeasurementMode(adxl345bSensorConfiguration_t *sensor);

/*!
 * @brief deactivates measurement mode
 *
 * @param sensor[in]  sensor to use
 * @return            return the error code (0 if everything passed)
 */
adxl345bErrorCode_t adxl345bDeactivateMeasurementMode(adxl345bSensorConfiguration_t *sensor);

/*!
 * @brief sets the sensor to full resolution mode
 *
 * @note actual length depends on value range: 10bit to 13bit -> always 4mG/LSB
 *
 * @param sensor[inout]  sensor to use
 * @return               return the error code (0 if everything passed)
 */
adxl345bErrorCode_t adxl345bSetFullResolutionMode(adxl345bSensorConfiguration_t *sensor);

/*!
 * @brief sets the sensor to 10bit mode
 *
 * @note length is always 10 Bit -> mG/LSB depends on value range
 *
 * @param sensor[inout]  sensor to use
 * @return               return the error code (0 if everything passed)
 */
adxl345bErrorCode_t adxl345bSetTenBitMode(adxl345bSensorConfiguration_t *sensor);

/*!
 * @brief changes the measurement range of the sensor
 *
 * @param sensor[inout]   sensor to use
 * @param rangeToUse[in]  range to be used for future measurements
 * @return                return the error code (0 if everything passed)
 */
adxl345bErrorCode_t adxl345bChangeMeasurementRange(adxl345bSensorConfiguration_t *sensor,
                                                   adxl345bRange_t rangeToUse);

// #############################################################################
// Self-Test and Calibration
// #############################################################################

/*!
 * @brief read the serial number from the sensor
 *
 * @param sensor[in]        sensor to use
 * @param serialNumber[out] memory where the serial number received from the sensor is stored
 * @return                  return the error code (0 if everything passed)
 */
adxl345bErrorCode_t adxl345bReadSerialNumber(adxl345bSensorConfiguration_t *sensor,
                                             uint8_t *serialNumber);

/*!
 * @brief performing the self-test procedure
 *
 * @attention Assumes that the sensor is not moved during the procedure.
 *
 * @attention After the procedure the output data rate and data format are changed!
 * @attention It will be reset to default configuration see 'adxl345bInit'.
 *
 * @param sensor[in]  sensor to use
 * @return            return the error code (0 if self-test passed)
 */
adxl345bErrorCode_t adxl345bPerformSelfTest(adxl345bSensorConfiguration_t *sensor);

/*!
 * @brief performing a self-calibration procedure
 *
 * @attention Assumes that the X, Y axes is at 0G and Z axes at 1G!
 *
 * @attention After the procedure the output data rate and data format are changed!
 * @attention It will be reset to default configuration see 'adxl345bInit'.
 *
 * @param sensor[in]  configuration for sensor to use
 * @return            return the error code (0 if self-calibration passed)
 */
adxl345bErrorCode_t adxl345bPerformSelfCalibration(adxl345bSensorConfiguration_t *sensor);

// #############################################################################
// Data Acquisition
// #############################################################################

/*
 * @brief check if an interrupt source is set
 *
 * @param sensor[in] configuration for sensor to use
 * @param mask[in]   bitmask of the interrupt source to check
 * @return           errorCode_t (0 if interrupt source is set)
 */
adxl345bErrorCode_t adxl345bCheckInterruptSet(adxl345bSensorConfiguration_t *sensor, uint8_t mask);

/*!
 * @brief convert raw data into LSB values of X,Y,Z axis
 *
 * @param data[in]     raw data (byte array) received from sensor
 * @param sample[out]  LSB values of the received raw data
 * @return             return the error code (0 if everything passed)
 */
adxl345bErrorCode_t adxl345bConvertBytesToLSB(adxl345bRawData_t data, adxl345bLsbSample_t *sample);

/*!
 * @brief convert raw data (LSB) into G values of X,Y,Z axis
 *
 * @param lsbValues[in]  LSB values of the received raw data
 * @param gValues[out]   G values of the received raw data
 * @return               return the error code (0 if everything passed)
 */
adxl345bErrorCode_t adxl345bConvertLSBToGValue(adxl345bLsbSample_t lsbValues,
                                               adxl345bGValueSample_t *gValues);

/*!
 * @brief read a single data triplet (LSB) from the sensor
 *
 * @attention assumes that the measurement mode is activated
 *
 * @note works best when FIFO is in bypass mode
 *
 * @param sensor[in]   configuration for sensor to use
 * @param sample[out]  raw bytes received from sensor
 * @return             return the error code (0 if everything passed)
 */
adxl345bErrorCode_t adxl345bGetSingleMeasurement(adxl345bSensorConfiguration_t *sensor,
                                                 adxl345bRawData_t *sample);

/*!
 * @brief read n data triplets (LSB) from the sensor
 *
 * @attention assumes that the measurement mode is activated
 *
 * @note works best when FIFO is in stream mode (threshold is ignored)
 *
 * @param sensor[in]           sensor to use
 * @param samples[out]         array of raw bytes received from the sensor
 * @param numberOfSamples[in]  number of required samples
 * @return                     return the error code (0 if everything passed)
 */
// adxl345bErrorCode_t adxl345bGetMultipleMeasurements(adxl345bSensorConfiguration_t *sensor,
//                                                     adxl345bRawData_t *samples,
//                                                     size_t numberOfSamples);

/*!
 * @brief read data triplets (LSB) from the sensor for n microseconds
 *
 * @attention assumes that the measurement mode is activated
 *
 * @throw If the buffer is to small to store all samples an error is returned
 *        and only the first samples will be stored!
 *
 * @note works best when FIFO is in stream mode (threshold is ignored)
 *
 * @param sensor[in]           sensor to use
 * @param timeToRead[in]       measuring-time in microseconds
 * @param samples[out]         array of raw bytes received from the sensor
 * @param numberOfSamples[in]  length of the raw data array
 * @return                     return the error code (0 if everything passed)
 */
// adxl345bErrorCode_t adxl345bGetMeasurementsForNMicroseconds(adxl345bSensorConfiguration_t
// *sensor,
//                                                             uint32_t timeToRead,
//                                                             adxl345bRawData_t *samples,
//                                                             size_t sizeOfSamples);

#endif /* ENV5_ADXL345B_PUBLIC_HEADER */
