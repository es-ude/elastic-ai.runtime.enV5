#ifndef ENV5_ADXL345B_INTERNAL_HEADER
#define ENV5_ADXL345B_INTERNAL_HEADER

/*! \b Datasheet: \n
 *      https://www.analog.com/media/en/technical-documentation/data-sheets/ADXL345.pdf
 * \n \b Quick \b Start \b Guide: \n
 *      https://www.analog.com/media/en/technical-documentation/application-notes/AN-1077.pdf
 * \n \n \b INTERRUPTS -> Not connected!\n \b FIFO -> Can not be used
 * effectively due to unconnected interrupt pins\n \b Sleep \b mode -> Can not
 * be used effectively due to unconnected interrupt pins\n \b Shock, Tap, Fall,
 * Activity \b detection -> Can not be used due to unconnected interrupt pins\n
 */

#include "include/eai/sensor/Adxl345bTypedefs.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct adxl345bRangeSetting {
    uint8_t settingForRange;
    uint8_t msbMask;   //!< used for converting raw data to LSB value
    float scaleFactor; //!< used for converting LSB value to G value
} adxl345bRangeSetting_t;

/*!
 * @brief function to read the data from the sensor
 *
 * @param i2cConfiguration[in] i2c Configuration to be used with sensor
 * @param registerToRead[in]       address of the register to start the read
 * from
 * @param responseBuffer[out]      memory to store the received data
 * @param sizeOfResponseBuffer[in] size of the response buffer
 * @return                         return the error code (0 if everything
 * passed)
 */
static adxl345bErrorCode_t adxl345bInternalReadDataFromSensor(adxl345bSensorConfiguration_t sensor,
                                                              adxl345bRegister_t registerToRead,
                                                              uint8_t *responseBuffer,
                                                              uint8_t sizeOfResponseBuffer);

/*!
 * @brief function to convert the raw value received from the sensor to a lsb value
 *
 * @param rawData[in]   raw data received from the sensor
 * @param lsbValue[out] LSB value
 * @return              return the error code (0 if everything passed)
 */
static adxl345bErrorCode_t adxl345bInternalConvertRawValueToLSB(const uint8_t rawData[2],
                                                                int *lsbValue);

/*!
 * @brief function to convert the lsb value received from the sensor to a actual float
 * value
 *
 * @param lsb[in] raw data received from the sensor
 * @param gValue[out] real world G value
 * @return            return the error code (0 if everything passed)
 */
static adxl345bErrorCode_t adxl345bInternalConvertLSBtoGValue(int lsb, float *gValue);

/*!
 * @brief combination of adxl345bInternalConvertRawValueToLSB(...) and
 * adxl345bInternalConvertLSBtoGValue(...)
 *
 * @param rawData[in] raw data received from the sensor
 * @param gValue[out] real world G value
 * @return            return the error code (0 if everything passed)
 */
static adxl345bErrorCode_t adxl345bInternalConvertRawValueToGValue(const uint8_t rawData[2],
                                                                   float *gValue);

/*!
 * @brief function to set the sensor into low power mode at 2G range with full
 * resolution
 * @IMPORTANT   We highly recommend using the "enV5_hw_configuration_rev_[x]" -library
 *
 * @return return the error code (0 if everything passed)
 */
static adxl345bErrorCode_t
adxl345bInternalWriteDefaultLowPowerConfiguration(adxl345bSensorConfiguration_t sensor);

/*!
 * @brief function to calculate the offset that should be passed to the sensor
 * @IMPORTANT   We highly recommend using the "enV5_hw_configuration_rev_[x]" -library
 *
 * @param measuredDelta[in] delta measured during self test
 * @param maxValue[in]      max value that would be accepted
 * @param minValue[in]      min value that would be accepted
 * @return                  8 bit two complement that should be passed to the
 * sensor as offset
 */
static int8_t adxl345bInternalCalculateCalibrationOffset(int measuredDelta, int maxValue,
                                                         int minValue);

/*!
 * @brief polls InterruptSource until specified interrupt occurs
 *
 * @IMPORTANT We highly recommend using the "enV5_hw_configuration_rev_[x]" -library
 *
 * @param sensor[in] configuration for sensor to use
 * @param mask[in] mask to specify which interrupt should be checked
 *
 * @return return the error code (0 if everything passed)
 * @note clears ADXL345B_REGISTER_INTERRUPT_SOURCE except DATA_READY, watermark, and overrun
 * information
 */
static adxl345bErrorCode_t
adxl345bInternalCheckInterruptSource(adxl345bSensorConfiguration_t sensor, uint8_t mask);

/*!
 * @brief reads raw data of xAxis,yAxis and zAxis
 *
 * @IMPORTANT   - We highly recommend using the "enV5_hw_configuration_rev_[x]" -library
 *              - there must be at least 5 μs between the end of reading the data registers and the
 * start of a new read
 *
 * @param sensor[in] configuration for sensor to use
 * @param dataResponseBuffer[out] memory where data received from the sensor is stored. needs to be
 * at least 6 bytes
 * @return return the error code (0 if everything passed)
 *
 * @note read only, raw data needs to be converted into g-values
 */
static adxl345bErrorCode_t adxl345bReadDataXYZ(adxl345bSensorConfiguration_t sensor,
                                               uint8_t *dataResponseBuffer);

/*! @brief
 * @IMPORTANT   We highly recommend using the "enV5_hw_configuration_rev_[x]" -library
 * @param sensor[in] configuration for sensor to use
 * @param dataResponseBuffer[out] memory where data received from the sensor is stored. needs to be
 * at least 6 bytes
 * @param maxFifoRead maximal Data you want to fetch from Fifo
 * @return return the error code (0 if everything passed)
 */
static adxl345bErrorCode_t fetchDataFromFifo(adxl345bSensorConfiguration_t sensor,
                                             uint8_t *dataResponseBuffer, uint8_t maxFifoRead);

/*! @brief
 * @IMPORTANT   We highly recommend using the "enV5_hw_configuration_rev_[x]" -library
 * @param sensor[in] configuration for sensor to use
 * @param sizeOfBuffer modulo 6 needs to be 0
 * @param remainder modulo 6 needs to be 0
 * @param maxFifoRead maximal Data you want to fetch from Fifo
 * @param buffer[out] memory where data received from the sensor is stored
 * @param fifoInformation Infomation which is stored in ADXL345B_FIFO_CONTROL
 * @return return the error code (0 if everything passed)
 */
static adxl345bErrorCode_t manageFifoDataRead(adxl345bSensorConfiguration_t sensor,
                                              int64_t sizeOfBuffer, uint8_t remainder,
                                              uint8_t maxFifoRead, uint8_t *buffer,
                                              uint8_t fifoInformation);

/*!
 * @brief disables selftest force by setting selftest bit to 0 in the ADXL345B_REGISTER_DATA_FORMAT
 * without changing other information stored in register
 * @IMPORTANT   We highly recommend using the "enV5_hw_configuration_rev_[x]" -library
 * @param sensor[in] configuration for sensor to use

 * @return return the error code (0 if everything passed)
 */
static adxl345bErrorCode_t
adxl345bInternalDisableSelftestForce(adxl345bSensorConfiguration_t sensor);

/*!
 * @brief enables selftest force by setting selftest bit to 1 in the ADXL345B_REGISTER_DATA_FORMAT
 * without changing other information stored in register
 * @IMPORTANT   We highly recommend using the "enV5_hw_configuration_rev_[x]" -library
 * @param sensor[in] configuration for sensor to use
 * @return return the error code (0 if everything passed)
 */
static adxl345bErrorCode_t
adxl345bInternalEnableSelftestForce(adxl345bSensorConfiguration_t sensor);

/*!
 * @brief stores selected Range Information in static parameter
 *
 * @param range range to be selected
 * @return return the error code (0 if everything passed)
 */
static adxl345bErrorCode_t adxl345bInternalSetSelectedRange(adxl345bRange_t range);

#endif /* ENV5_ADXL345B_INTERNAL_HEADER */
