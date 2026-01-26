#ifndef ENV5_ADXL345B_INTERNAL_HEADER
#define ENV5_ADXL345B_INTERNAL_HEADER

/* Datasheet:
 *      https://www.analog.com/media/en/technical-documentation/data-sheets/ADXL345.pdf
 * Quick Start Guide:
 *      https://www.analog.com/media/en/technical-documentation/application-notes/AN-1077.pdf
 *
 * Interrupt pins -> Not connected!
 * FIFO -> Can not be used effectively due to unconnected interrupt pins
 * Sleep mode -> Can not be used effectively due to unconnected interrupt pins
 * Tap, Fall, Activity detection -> Can not be used effectively due to unconnected interrupt pins
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "include/eai/sensor/Adxl345bTypedefs.h"

// #############################################################################
// Configuration
// #############################################################################

/**
 * @brief write the default configuration to the sensor
 *
 * @note This function sets the sensor to default configuration:
 * @note   - Output Data Rate: 12.5Hz, Low Power Mode
 * @note   - Measurement Mode: Deactivated
 * @note   - Interrupts: All Disabled
 * @note   - Range: ±2G, 10bit mode
 *
 * @param sensor[in]  sensor to use
 * @return            return the error code (0 if everything passed)
 */
adxl345bErrorCode_t
adxl345bInternalWriteDefaultConfiguration(adxl345bSensorConfiguration_t *sensor);

/*!
 * @brief enable internal self-test force
 *
 * @param sensor[in]  sensor to use
 * @return            return the error code (0 if everything passed)
 */
adxl345bErrorCode_t adxl345bInternalEnableSelfTestForce(adxl345bSensorConfiguration_t *sensor);

/*!
 * @brief disable internal self-test force
 *
 * @param sensor[in]  sensor to use
 * @return            return the error code (0 if everything passed)
 */
adxl345bErrorCode_t adxl345bInternalDisableSelfTestForce(adxl345bSensorConfiguration_t *sensor);

// #############################################################################
// Calibration
// #############################################################################

/*!
 * @brief function to calculate the offset that should be passed to the sensor
 *
 * @param measured[in]  measured value representing the offset to 0G LSB
 * @return              offset as 8 bit two's complement
 */
int8_t adxl345bInternalCalculateCalibrationOffset(int16_t measured);

// #############################################################################
// Data Conversion
// #############################################################################

/*!
 * @brief convert two's complement to a LSB value (16 bit signed integer)
 *
 * @attention expects data to be right justified
 *
 * @param valueRange[in]        range the sensor is configured to
 * @param isFullResolution[in]  whether the sensor is configured in full resolution mode
 * @param rawValue[in]          raw data received from the sensor ([0]: LSB, [1]: MSB)
 * @return                      LSB value
 */
int16_t adxl345bInternalConvertRawDataToLsbValue(adxl345bRange_t valueRange, bool isFullResolution,
                                                 uint8_t rawValue[2]);

/*!
 * @brief convert the lsb value received from the sensor to a actual g value
 *
 * @param valueRange[in]        range the sensor is configured to
 * @param isFullResolution[in]  whether the sensor is configured in full resolution mode
 * @param rawValue[in]          raw data received from the sensor
 * @return                      G value
 */
float adxl345bInternalConvertLSBtoGValue(adxl345bRange_t valueRange, bool isFullResolution,
                                         int rawValue);

// #############################################################################
// Data Handling
// #############################################################################

/*!
 * @brief read x-, y-, and z-axis data from the sensor
 *
 * @param sensor[in]   configuration for sensor to use
 * @param rawData[out] memory where data received from the sensor is stored.
 *
 * @return             return the error code (0 if everything passed)
 */
adxl345bErrorCode_t adxl345bInternalReadDataXYZ(adxl345bSensorConfiguration_t *sensor,
                                                uint8_t rawData[6]);

/*!
 * @brief clear FIFO buffer of the sensor
 *
 * @param sensor[in]  configuration for sensor to use
 * @return            return the error code (0 if everything passed)
 */
adxl345bErrorCode_t adxl345bInternalClearFifoBuffer(adxl345bSensorConfiguration_t *sensor);

#endif /* ENV5_ADXL345B_INTERNAL_HEADER */
