#define SOURCE_FILE "ADXL345B-LIB"

#include "include/eai/sensor/Adxl345b.h"
#include "Adxl345bInternal.h"
#include "eai/Common.h"
#include "eai/hal/I2c.h"
#include "eai/hal/Sleep.h"
#include "eai/hal/Time.h"
#include "include/eai/sensor//Adxl345bTypedefs.h"

#include "math.h"

/* region CONSTANTS */

/*! For more Information about msbMask see Figure 49 on Page 32 of 36 in Datasheet  */
/*
 * const static adxl345bRangeSetting_t adxl345b_2g_range2V5 = {0b00001000, 0b00000011, 0.0039f,
 * 0.0039f}; const static adxl345bRangeSetting_t adxl345b_4g_range2V5 = {0b00001001, 0b00000111,
 * 0.0078f, 0.0039f}; const static adxl345bRangeSetting_t adxl345b_8g_range2V5 = {0b00001010,
 * 0b00001111, 0.0156f, 0.0039f}; const static adxl345bRangeSetting_t adxl345b_16g_range2V5 =
 * {0b00001011, 0b00111111, 0.0312f, 0.[48;40;187;960;1870t0039f};
 */
const static adxl345bRangeSetting_t adxl345b_2g_range3V3 = {0b00001000, 0b00000011, 0.00391,
                                                            0.00377f};
const static adxl345bRangeSetting_t adxl345b_4g_range3V3 = {0b00001001, 0b00000111, 0.0078125,
                                                            0.00377f};
const static adxl345bRangeSetting_t adxl345b_8g_range3V3 = {0b00001010, 0b00001111, 0.015625,
                                                            0.00377f};
const static adxl345bRangeSetting_t adxl345b_16g_range3V3 = {0b00001011, 0b00111111, 0.03125,
                                                             0.00377f};

//! measurement range configuration
static adxl345bRangeSetting_t adxl345bSelectedRange;

/* endregion CONSTANTS */

/* region HEADER FUNCTION IMPLEMENTATIONS */

adxl345bErrorCode_t adxl345bInit(adxl345bSensorConfiguration_t sensor) {
    /* sleep to make sure the sensor is fully initialized */
    sleep_for_ms(2);

    /* Check if sensor ADXL345B is on Bus by requesting serial number without processing */
    uint8_t sizeOfCommandBuffer = 1;
    uint8_t commandBuffer[sizeOfCommandBuffer];
    commandBuffer[0] = ADXL345B_REGISTER_DEVICE_ID;

    i2cErrorCode_t i2cErrorCode = i2cWriteCommand(sensor.i2c_host, sensor.i2c_slave_address,
                                                  commandBuffer, sizeOfCommandBuffer);
    if (i2cErrorCode != I2C_NO_ERROR) {
        PRINT("sensor not available on bus; i2c_ERROR: %02X", i2cErrorCode);
        return ADXL345B_INIT_ERROR;
    }

    adxl345bErrorCode_t errorCode = adxl345bInternalWriteDefaultLowPowerConfiguration(sensor);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }

    return ADXL345B_NO_ERROR;
}

adxl345bErrorCode_t adxl345bWriteConfigurationToSensor(adxl345bSensorConfiguration_t sensor,
                                                       adxl345bRegister_t registerToWrite,
                                                       adxl345bConfig_t config) {
    uint8_t sizeOfCommandBuffer = 2;
    uint8_t commandBuffer[sizeOfCommandBuffer];
    commandBuffer[0] = registerToWrite;
    commandBuffer[1] = config;

    i2cErrorCode_t i2cErrorCode = i2cWriteCommand(sensor.i2c_host, sensor.i2c_slave_address,
                                                  commandBuffer, sizeOfCommandBuffer);
    if (i2cErrorCode != I2C_NO_ERROR) {
        PRINT("sensor not available on bus; i2c_ERROR: %02X", i2cErrorCode);
        return ADXL345B_CONFIGURATION_ERROR;
    }

    return ADXL345B_NO_ERROR;
}

adxl345bErrorCode_t adxl345bSetFIFOMode(adxl345bSensorConfiguration_t sensor, uint8_t fifoMode,
                                        uint16_t samplesForTrigger) {
    uint8_t fifoConfiguration =
        (fifoMode & ADXL345B_BITMASK_FIFOMODE) | (samplesForTrigger & ADXL345B_BITMASK_SAMPLES);
    adxl345bErrorCode_t errorCode =
        adxl345bWriteConfigurationToSensor(sensor, ADXL345B_FIFO_CONTROL, fifoConfiguration);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }
    return ADXL345B_NO_ERROR;
}

adxl345bErrorCode_t adxl345bChangeMeasurementRange(adxl345bSensorConfiguration_t sensor,
                                                   adxl345bRange_t newRange) {
    adxl345bErrorCode_t errorCode = adxl345bInternalSetSelectedRange(newRange);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }
    uint8_t currentDataFormatSetting;
    errorCode = adxl345bInternalReadDataFromSensor(sensor, ADXL345B_REGISTER_DATA_FORMAT,
                                                   &currentDataFormatSetting, 1);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }
    /* reset/clear Range & justify Bit */
    currentDataFormatSetting =
        currentDataFormatSetting & ~ADXL345B_BITMASK_RANGE & ~ADXL345B_BITMASK_JUSTIFY;
    /* set new Range */
    uint8_t newDataFormatSetting =
        currentDataFormatSetting || adxl345bSelectedRange.settingForRange;

    errorCode = adxl345bWriteConfigurationToSensor(sensor, ADXL345B_REGISTER_DATA_FORMAT,
                                                   newDataFormatSetting);
    adxl345bSetFullResolutionMode(sensor);
    return errorCode;
}

adxl345bErrorCode_t adxl345bReadSerialNumber(adxl345bSensorConfiguration_t sensor,
                                             uint8_t *serialNumber) {
    uint8_t sizeOfResponseBuffer = 1;
    uint8_t responseBuffer[sizeOfResponseBuffer];

    adxl345bErrorCode_t adxl345bErrorCode = adxl345bInternalReadDataFromSensor(
        sensor, ADXL345B_REGISTER_DEVICE_ID, responseBuffer, sizeOfResponseBuffer);
    if (adxl345bErrorCode != ADXL345B_NO_ERROR) {
        PRINT("sensor not available on i2c bus; adxl345b_ERROR: %02X", adxl345bErrorCode);
        return adxl345bErrorCode;
    }

    *serialNumber = responseBuffer[0];

    return ADXL345B_NO_ERROR;
}

adxl345bErrorCode_t adxl345bGetSingleMeasurement(adxl345bSensorConfiguration_t sensor,
                                                 uint8_t *rawData) {
    adxl345bErrorCode_t errorCode;
    errorCode = adxl345bInternalCheckInterruptSource(sensor, ADXL345B_BITMASK_DATA_READY);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }

    errorCode = adxl345bReadDataXYZ(sensor, rawData);
    return errorCode;
}

adxl345bErrorCode_t adxl345bGetMeasurementsForNMilliseconds(adxl345bSensorConfiguration_t sensor,
                                                            uint8_t *rawData, uint32_t milliseconds,
                                                            uint32_t *sizeOfRawData) {
    adxl345bErrorCode_t errorCode = ADXL345B_NO_ERROR;

    uint32_t startTime;
    uint64_t endTime;
    uint32_t counter = 0;
    uint32_t size = *sizeOfRawData;
    size -= (size) % 6;

    /* read and set configuration*/
    uint8_t fifoInformation;
    errorCode =
        adxl345bInternalReadDataFromSensor(sensor, ADXL345B_FIFO_CONTROL, &fifoInformation, 1);
    if (errorCode != ADXL345B_NO_ERROR) {
        PRINT_DEBUG("reading FIFO_CONTROL failed");
        return errorCode;
    }
    uint8_t samplesInFifo = (fifoInformation & ADXL345B_BITMASK_SAMPLES) + 1;

    if ((fifoInformation & ADXL345B_BITMASK_FIFOMODE) == ADXL345B_FIFOMODE_BYPASS) {
        PRINT_DEBUG("Start reading in BYPASS_MODE");
        startTime = timeUs32() / 1000;
        endTime = (uint64_t)(milliseconds * 1000) + startTime;
        while (true) {
            if (endTime <= (timeUs32() / 1000)) {
                *sizeOfRawData = counter;
                return errorCode;
            } else if ((size - counter) <= 6) {
                PRINT_DEBUG(
                    "sizeOfRawData is too small to store all data measured in given milliseconds");
                *sizeOfRawData = counter;
                return errorCode;
            }
            adxl345bGetSingleMeasurement(sensor, rawData + counter);
            counter += 6;
            // sleep 5 μs to ensure data is ready
            sleep_for_us(5);
        }
    }
    uint8_t maxFifoRead = samplesInFifo * 6;
    uint8_t remainder = 0;
    if (size >= maxFifoRead) {
        remainder = size % maxFifoRead;
    } else {
        maxFifoRead = size;
    }
    PRINT_DEBUG("Start reading STREAM-, TRIGGER- OR FIFO-MODE");
    startTime = timeUs32() / 1000;
    endTime = (uint64_t)(milliseconds) * 1000 + startTime;

    while (true) {
        if (endTime <= (timeUs32() / 1000)) {
            *sizeOfRawData = counter;
            return errorCode;
        } else if ((size - counter) <= 6) {
            PRINT_DEBUG(
                "sizeOfRawData is too small to store all data measured in given milliseconds");
            if (size != counter) {
                PRINT_DEBUG("some test should fail in this case. please report issue");
                return ADXL345B_UNDEFINED_ERROR;
            }
            *sizeOfRawData = counter;
            return errorCode;
        }
        errorCode = manageFifoDataRead(sensor, size - counter, remainder, maxFifoRead,
                                       rawData + counter, fifoInformation);
        counter += maxFifoRead;
        if (errorCode != ADXL345B_NO_ERROR) {
            return errorCode;
        }
    }
    PRINT_DEBUG("Something went horribly wrong");
    return errorCode;
}

adxl345bErrorCode_t adxl345bGetMultipleMeasurements(adxl345bSensorConfiguration_t sensor,
                                                    uint8_t *rawData, uint32_t sizeOfRawData) {
    adxl345bErrorCode_t errorCode;

    int64_t counter = 0;
    sizeOfRawData -= (sizeOfRawData % 6);

    uint8_t fifoInformation;
    errorCode =
        adxl345bInternalReadDataFromSensor(sensor, ADXL345B_FIFO_CONTROL, &fifoInformation, 1);
    if (errorCode != ADXL345B_NO_ERROR) {
        PRINT_DEBUG("read FIFO_CONTROL failed");
        return errorCode;
    }

    uint8_t samplesInFifo = (fifoInformation & ADXL345B_BITMASK_SAMPLES) + 1;
    uint8_t maxFifoRead = samplesInFifo * 6;

    uint8_t remainder = 0;
    if (sizeOfRawData >= maxFifoRead) {
        remainder = sizeOfRawData % maxFifoRead;
    } else {
        maxFifoRead = sizeOfRawData;
    }
    PRINT_DEBUG("Start reading");
    while (((int64_t)sizeOfRawData - counter) >= 6) {
        errorCode = manageFifoDataRead(sensor, sizeOfRawData - counter, remainder, maxFifoRead,
                                       rawData + counter, fifoInformation);
        counter += maxFifoRead;
        if (errorCode != ADXL345B_NO_ERROR) {
            return errorCode;
        }
    }
    PRINT_DEBUG("done reading");
    return errorCode;
}

adxl345bErrorCode_t adxl345bConvertDataXYZ(adxl345bSensorConfiguration_t sensor, float *xAxis,
                                           float *yAxis, float *zAxis, uint8_t *responseBuffer) {
    adxl345bErrorCode_t errorCode;
    errorCode = adxl345bInternalConvertRawValueToGValue(sensor, &responseBuffer[0], xAxis);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }
    errorCode = adxl345bInternalConvertRawValueToGValue(sensor, &responseBuffer[2], yAxis);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }
    errorCode = adxl345bInternalConvertRawValueToGValue(sensor, &responseBuffer[4], zAxis);
    return errorCode;
}

adxl345bErrorCode_t adxl345bResetTrigger(adxl345bSensorConfiguration_t sensor,
                                         uint8_t fifoInformation) {
    adxl345bErrorCode_t errorCode;
    PRINT_DEBUG("reset trigger");
    errorCode = adxl345bSetFIFOMode(sensor, ADXL345B_FIFOMODE_BYPASS,
                                    fifoInformation & ADXL345B_BITMASK_SAMPLES);
    if (errorCode != ADXL345B_NO_ERROR) {
        PRINT_DEBUG("reset trigger failed");
        return errorCode;
    }
    errorCode = adxl345bSetFIFOMode(sensor, ADXL345B_FIFOMODE_TRIGGER,
                                    fifoInformation & ADXL345B_BITMASK_SAMPLES);
    if (errorCode != ADXL345B_NO_ERROR) {
        PRINT_DEBUG("reset trigger failed");
        return errorCode;
    }
    return errorCode;
}

adxl345bErrorCode_t adxl345bActivateMeasurementMode(adxl345bSensorConfiguration_t sensor) {
    adxl345bErrorCode_t errorCode = adxl345bInternalClearRegisterBits(
        sensor, ADXL345B_REGISTER_POWER_CONTROL, ADXL345B_BITMASK_MEASURE);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }

    errorCode = adxl345bInternalSetRegisterBits(sensor, ADXL345B_REGISTER_POWER_CONTROL,
                                                ADXL345B_BITMASK_MEASURE);
    return errorCode;
}

adxl345bErrorCode_t adxl345bPerformSelfTest(adxl345bSensorConfiguration_t sensor, int *averageX,
                                            int *averageY, int *averageZ) {
    adxl345bErrorCode_t errorCode;
    errorCode = adxl345bActivateMeasurementMode(sensor);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }
    errorCode =
        adxl345bWriteConfigurationToSensor(sensor, ADXL345B_REGISTER_BW_RATE, ADXL345B_BW_RATE_100);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }
    errorCode = adxl345bChangeMeasurementRange(sensor, ADXL345B_16G_RANGE);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }
    errorCode = adxl345bInternalEnableSelftestForce(sensor);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }

    sleep_for_ms(2);

    /* collect samples with force */
    int numberOfSamples = 10;
    int samplesWithForce[numberOfSamples][3];
    int counter = 0;
    while (counter < numberOfSamples) {
        uint8_t interrupt_source;
        errorCode = adxl345bInternalReadDataFromSensor(sensor, ADXL345B_REGISTER_INTERRUPT_SOURCE,
                                                       &interrupt_source, 1);
        if (errorCode != ADXL345B_NO_ERROR) {
            return errorCode;
        }

        if ((interrupt_source & ADXL345B_BITMASK_DATA_READY) == ADXL345B_BITMASK_DATA_READY) {
            uint8_t sizeOfResponseBuffer = 6;
            uint8_t responseBuffer[sizeOfResponseBuffer];

            errorCode = adxl345bInternalReadDataFromSensor(sensor, ADXL345B_REGISTER_DATA_X,
                                                           responseBuffer, sizeOfResponseBuffer);
            if (errorCode != ADXL345B_NO_ERROR) {
                return errorCode;
            }

            int dataX, dataY, dataZ;
            errorCode = adxl345bInternalConvertRawValueToLSB(sensor, &responseBuffer[0], &dataX);
            if (errorCode != ADXL345B_NO_ERROR) {
                return errorCode;
            }
            errorCode = adxl345bInternalConvertRawValueToLSB(sensor, &responseBuffer[2], &dataY);
            if (errorCode != ADXL345B_NO_ERROR) {
                return errorCode;
            }
            errorCode = adxl345bInternalConvertRawValueToLSB(sensor, &responseBuffer[4], &dataZ);
            if (errorCode != ADXL345B_NO_ERROR) {
                return errorCode;
            }

            samplesWithForce[counter][0] = dataX;
            samplesWithForce[counter][1] = dataY;
            samplesWithForce[counter][2] = dataZ;

            sleep_for_ms(20);
            counter++;
        }
    }

    /* calculate average of samples with force */
    int sumSamplesWithForceX = 0;
    int sumSamplesWithForceY = 0;
    int sumSamplesWithForceZ = 0;
    for (int index = 0; index < numberOfSamples; index++) {
        sumSamplesWithForceX += samplesWithForce[index][0];
        sumSamplesWithForceY += samplesWithForce[index][1];
        sumSamplesWithForceZ += samplesWithForce[index][2];
    }
    int avgSampleWithForceX = sumSamplesWithForceX / numberOfSamples;
    int avgSampleWithForceY = sumSamplesWithForceY / numberOfSamples;
    int avgSampleWithForceZ = sumSamplesWithForceZ / numberOfSamples;

    errorCode = adxl345bInternalDisableSelftestForce(sensor);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }

    sleep_for_ms(2);

    /* collect samples without force */
    int samplesWithoutForce[numberOfSamples][3];
    counter = 0;
    while (counter < numberOfSamples) {
        uint8_t interrupt_source;
        errorCode = adxl345bInternalReadDataFromSensor(sensor, ADXL345B_REGISTER_INTERRUPT_SOURCE,
                                                       &interrupt_source, 1);
        if (errorCode != ADXL345B_NO_ERROR) {
            return errorCode;
        }

        if (interrupt_source & ADXL345B_BITMASK_DATA_READY) {
            uint8_t sizeOfResponseBuffer = 6;
            uint8_t responseBuffer[sizeOfResponseBuffer];
            errorCode = adxl345bInternalReadDataFromSensor(sensor, ADXL345B_REGISTER_DATA_X,
                                                           responseBuffer, sizeOfResponseBuffer);
            if (errorCode != ADXL345B_NO_ERROR) {
                return errorCode;
            }

            int dataX, dataY, dataZ;
            errorCode = adxl345bInternalConvertRawValueToLSB(sensor, &responseBuffer[0], &dataX);
            if (errorCode != ADXL345B_NO_ERROR) {
                return errorCode;
            }
            errorCode = adxl345bInternalConvertRawValueToLSB(sensor, &responseBuffer[2], &dataY);
            if (errorCode != ADXL345B_NO_ERROR) {
                return errorCode;
            }
            errorCode = adxl345bInternalConvertRawValueToLSB(sensor, &responseBuffer[4], &dataZ);
            if (errorCode != ADXL345B_NO_ERROR) {
                return errorCode;
            }

            samplesWithoutForce[counter][0] = dataX;
            samplesWithoutForce[counter][1] = dataY;
            samplesWithoutForce[counter][2] = dataZ;

            sleep_for_ms(20);
            counter++;
        }
    }

    /* calculate average of samples without force */
    int sumSamplesWithoutForceX = 0;
    int sumSamplesWithoutForceY = 0;
    int sumSamplesWithoutForceZ = 0;
    for (int index = 0; index < numberOfSamples; index++) {
        sumSamplesWithoutForceX += samplesWithoutForce[index][0];
        sumSamplesWithoutForceY += samplesWithoutForce[index][1];
        sumSamplesWithoutForceZ += samplesWithoutForce[index][2];
    }
    int avgSampleWithoutForceX = sumSamplesWithoutForceX / numberOfSamples;
    int avgSampleWithoutForceY = sumSamplesWithoutForceY / numberOfSamples;
    int avgSampleWithoutForceZ = sumSamplesWithoutForceZ / numberOfSamples;

    adxl345bInternalWriteDefaultLowPowerConfiguration(sensor);

    /* compare average to datasheet */
    int deltaOfAverageX = avgSampleWithForceX - avgSampleWithoutForceX;

    int deltaOfAverageY = avgSampleWithForceY - avgSampleWithoutForceY;

    int deltaOfAverageZ = avgSampleWithForceZ - avgSampleWithoutForceZ;

    /* See Table 15. Self-Test Output in LSB for FULL RESOLUTION
     [multiply X and Y by 1.77 and Z by 1.47 -because of 3.3V- (See Table 14)]
     X min 50 LSB * 1.77 = 88.5 LSB
     X max 540 LSB * 1.77 = 955.8 LSB
     Y min -540 LSB * 1.77 = −955.8 LSB
     Y max -50 LSB * 1.77 = −88.5 LSB
     Z min 75 LSB * 1.47 = 110.25 LSB
     Z max 875 LSB * 1.47 = 1286.25 LSB
*/

    float minDeltaX = 88.5, maxDeltaX = 955.8;
    float minDeltaY = -955.8, maxDeltaY = -88.5;
    float minDeltaZ = 110.25, maxDeltaZ = 1286.25;

    if (!(minDeltaX <= deltaOfAverageX && deltaOfAverageX <= maxDeltaX)) {
        return ADXL345B_SELF_TEST_FAILED;
    }

    if (!(minDeltaY <= deltaOfAverageY && deltaOfAverageY <= maxDeltaY)) {
        return ADXL345B_SELF_TEST_FAILED;
    }

    if (!(minDeltaZ <= deltaOfAverageZ && deltaOfAverageZ <= maxDeltaZ)) {
        return ADXL345B_SELF_TEST_FAILED;
    }

    return ADXL345B_NO_ERROR;
}

adxl345bErrorCode_t adxl345bRunSelfCalibration(adxl345bSensorConfiguration_t sensor) {
    int xMeasuredAtZeroG, yMeasuredAtZeroG, zMeasuredAtOneG;
    adxl345bErrorCode_t errorCode =
        adxl345bPerformSelfTest(sensor, &xMeasuredAtZeroG, &yMeasuredAtZeroG, &zMeasuredAtOneG);
    if (errorCode != ADXL345B_SELF_TEST_FAILED) {
        return errorCode;
    }
    errorCode =
        adxl345bWriteConfigurationToSensor(sensor, ADXL345B_REGISTER_BW_RATE, ADXL345B_BW_RATE_800);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }
    int8_t offsetXYZ[3] = {0};
    errorCode = adxl345bInternalCalculateCalibrationOffset(
        sensor, xMeasuredAtZeroG, yMeasuredAtZeroG, zMeasuredAtOneG, &offsetXYZ[0], &offsetXYZ[1],
        &offsetXYZ[2]);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }
    errorCode = adxl345bWriteConfigurationToSensor(sensor, ADXL345B_OFFSET_X, offsetXYZ[0]);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }
    errorCode = adxl345bWriteConfigurationToSensor(sensor, ADXL345B_OFFSET_Y, offsetXYZ[1]);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }
    errorCode = adxl345bWriteConfigurationToSensor(sensor, ADXL345B_OFFSET_Z, offsetXYZ[2]);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }
    PRINT_DEBUG("offsetX %d LSB", offsetXYZ[0]);
    PRINT_DEBUG("offsetY %d LSB", offsetXYZ[1]);
    PRINT_DEBUG("offsetZ %d LSB", offsetXYZ[2]);
    errorCode =
        adxl345bPerformSelfTest(sensor, &xMeasuredAtZeroG, &yMeasuredAtZeroG, &zMeasuredAtOneG);
    if (errorCode == ADXL345B_SELF_TEST_FAILED) {
        return ADXL345B_CALIBRATION_ERROR;
    }
    return errorCode;
}

adxl345bErrorCode_t adxl345bSetFullResolutionMode(adxl345bSensorConfiguration_t sensor) {
    adxl345bErrorCode_t errorCode = adxl345bInternalSetRegisterBits(
        sensor, ADXL345B_REGISTER_DATA_FORMAT, ADXL345B_BITMASK_FULL_RES);
    return errorCode;
}

adxl345bErrorCode_t adxl345bSetTenBitMode(adxl345bSensorConfiguration_t sensor) {
    adxl345bErrorCode_t errorCode = adxl345bInternalClearRegisterBits(
        sensor, ADXL345B_REGISTER_DATA_FORMAT, ADXL345B_BITMASK_FULL_RES);
    return errorCode;
}

/* endregion HEADER FUNCTION IMPLEMENTATIONS */

/* region HELPER FUNCTION IMPLEMENTATIONS */

static adxl345bErrorCode_t adxl345bReadDataXYZ(adxl345bSensorConfiguration_t sensor,
                                               uint8_t *dataResponseBuffer) {
    adxl345bErrorCode_t errorCode;
    uint8_t sizeOfResponseBuffer = 6;
    uint8_t responseBuffer[sizeOfResponseBuffer];
    errorCode = adxl345bInternalReadDataFromSensor(sensor, ADXL345B_REGISTER_DATA_X, responseBuffer,
                                                   sizeOfResponseBuffer);
    for (uint8_t index = 0; index < sizeOfResponseBuffer; index++) {
        dataResponseBuffer[index] = responseBuffer[index];
    }
    return errorCode;
}

static adxl345bErrorCode_t
adxl345bInternalCheckInterruptSource(adxl345bSensorConfiguration_t sensor, uint8_t mask) {
    adxl345bErrorCode_t errorCode;
    uint8_t interruptSource;
    do {
        errorCode = adxl345bInternalReadDataFromSensor(sensor, ADXL345B_REGISTER_INTERRUPT_SOURCE,
                                                       &interruptSource, 1);
        if (errorCode != ADXL345B_NO_ERROR) {
            PRINT_DEBUG("reading Register_Interrupt_Source failed");
            return errorCode;
        }
    } while (!(interruptSource & mask));
    return errorCode;
}
static adxl345bErrorCode_t adxl345bInternalSetSelectedRange(adxl345bRange_t range) {
    switch (range) {
    case ADXL345B_2G_RANGE:
        adxl345bSelectedRange = adxl345b_2g_range3V3;
        break;
    case ADXL345B_4G_RANGE:
        adxl345bSelectedRange = adxl345b_4g_range3V3;
        break;
    case ADXL345B_8G_RANGE:
        adxl345bSelectedRange = adxl345b_8g_range3V3;
        break;
    case ADXL345B_16G_RANGE:
        adxl345bSelectedRange = adxl345b_16g_range3V3;
        break;
    default:
        adxl345bSelectedRange = adxl345b_2g_range3V3;
        return ADXL345B_RANGE_ERROR;
    }
    return ADXL345B_NO_ERROR;
}

static adxl345bErrorCode_t manageFifoDataRead(adxl345bSensorConfiguration_t sensor,
                                              int64_t sizeOfBuffer, uint8_t remainder,
                                              uint8_t maxFifoRead, uint8_t *buffer,
                                              uint8_t fifoInformation) {
    adxl345bErrorCode_t errorCode = ADXL345B_NO_ERROR;

    if (sizeOfBuffer == remainder) {
        PRINT_DEBUG("prepare reading remaining samples");
        maxFifoRead = remainder; // last iteration needs other value
        uint8_t fifoStatus;

        do {
            errorCode =
                adxl345bInternalReadDataFromSensor(sensor, ADXL345B_FIFO_STATUS, &fifoStatus, 1);
            // sleep 5 μs to ensure data is ready
            sleep_for_us(5);
            if (errorCode != ADXL345B_NO_ERROR) {
                return errorCode;
            }
        } while ((fifoStatus & ADXL345B_BITMASK_ENTRIES) <
                 (remainder / 6)); // check entries until enough data available
        PRINT_DEBUG("start reading remaining samples");
    } else {
        errorCode = adxl345bInternalCheckInterruptSource(sensor, ADXL345B_BITMASK_WATERMARK);

        if (errorCode != ADXL345B_NO_ERROR) {
            PRINT_DEBUG("checking watermark failed");
            return errorCode;
        }
    }
    /* read Data */
    errorCode = fetchDataFromFifo(sensor, buffer, maxFifoRead);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }

    /*reset trigger if needed*/
    if ((fifoInformation & ADXL345B_BITMASK_FIFOMODE) == ADXL345B_FIFOMODE_TRIGGER) {
        errorCode = adxl345bResetTrigger(sensor, fifoInformation);
        if (errorCode != ADXL345B_NO_ERROR) {
            PRINT_DEBUG("reset trigger failed");
            return errorCode;
        }
    }
    return errorCode;
}

static adxl345bErrorCode_t fetchDataFromFifo(adxl345bSensorConfiguration_t sensor,
                                             uint8_t *dataResponseBuffer, uint8_t maxFifoRead) {
    adxl345bErrorCode_t errorCode = ADXL345B_NO_ERROR;
    for (uint8_t i = 0; i < maxFifoRead; i += 6) {
        errorCode = adxl345bReadDataXYZ(sensor, dataResponseBuffer + i);

        // sleep 5 μs to ensure data is ready
        sleep_for_us(5);

        if (errorCode != ADXL345B_NO_ERROR) {
            PRINT_DEBUG("ReadDataXYZ failed");
            return errorCode;
        }
    }
    return errorCode;
}

static adxl345bErrorCode_t adxl345bInternalReadDataFromSensor(adxl345bSensorConfiguration_t sensor,
                                                              adxl345bRegister_t registerToRead,
                                                              uint8_t *responseBuffer,
                                                              uint8_t sizeOfResponseBuffer) {
    uint8_t sizeOfCommandBuffer = 1;
    uint8_t commandBuffer[sizeOfCommandBuffer];
    commandBuffer[0] = registerToRead;

    PRINT_DEBUG("requesting data from sensor");
    i2cErrorCode_t i2cErrorCode = i2cWriteCommand(sensor.i2c_host, sensor.i2c_slave_address,
                                                  commandBuffer, sizeOfCommandBuffer);
    if (i2cErrorCode != I2C_NO_ERROR) {
        PRINT_DEBUG("sending request failed");
        return ADXL345B_SEND_COMMAND_ERROR;
    }

    PRINT_DEBUG("receiving data from sensor");
    i2cErrorCode = i2cReadData(sensor.i2c_host, sensor.i2c_slave_address, responseBuffer,
                               sizeOfResponseBuffer);
    if (i2cErrorCode != I2C_NO_ERROR) {
        PRINT_DEBUG("receiving data failed");
        return ADXL345B_RECEIVE_DATA_ERROR;
    }

    PRINT_DEBUG("successful received data from sensor");
    return ADXL345B_NO_ERROR;
}

static adxl345bErrorCode_t
adxl345bInternalConvertRawValueToLSB(adxl345bSensorConfiguration_t sensor, const uint8_t rawData[2],
                                     int *lsbValue) {
    uint8_t currentDataFormatSetting;
    adxl345bErrorCode_t errorCode = adxl345bInternalReadDataFromSensor(
        sensor, ADXL345B_REGISTER_DATA_FORMAT, &currentDataFormatSetting, 1);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }

    uint8_t msbRangeMask = adxl345bSelectedRange.msbMask;

    if ((currentDataFormatSetting & ADXL345B_BITMASK_FULL_RES) != ADXL345B_BITMASK_FULL_RES) {
        msbRangeMask = 0b00000011; // check Figure 49. in Documentation
    }
    PRINT_DEBUG("convert raw data to LSB value");
    if (rawData[1] <= (msbRangeMask >> 1)) {
        PRINT_DEBUG("entered positive case");
        uint16_t rawValue =
            ((uint16_t)(rawData[1] & (msbRangeMask >> 1)) << 8) | (uint16_t)rawData[0];
        *lsbValue = (int)rawValue;
    } else {
        PRINT_DEBUG("entered negative case");
        /* manual translation needed
         *
         * 1. revert two complement: number minus 1 and flip bits
         * 2. convert to int and multiply with -1
         */
        uint16_t rawValue =
            ((uint16_t)(rawData[1] & (msbRangeMask >> 1)) << 8) | (uint16_t)rawData[0];
        rawValue = (rawValue - 0x0001) ^ ((msbRangeMask >> 1) << 8 | 0x00FF);
        *lsbValue = (-1) * (int)rawValue;
    }

    PRINT_DEBUG("conversion successful");
    return ADXL345B_NO_ERROR;
}

static adxl345bErrorCode_t adxl345bInternalConvertLSBtoGValue(int lsb, float *gValue) {
    PRINT_DEBUG("convert LSB value to G value");
    float realValue = (float)lsb * adxl345bSelectedRange.tenBitScaleFactor;
    *gValue = realValue;
    PRINT_DEBUG("LSB: %i, G value: %f", lsb, realValue);

    return ADXL345B_NO_ERROR;
}

static adxl345bErrorCode_t
adxl345bInternalConvertRawValueToGValue(adxl345bSensorConfiguration_t sensor,
                                        const uint8_t rawData[2], float *gValue) {
    PRINT_DEBUG("converting raw data to G value");
    int intermediateLsb;
    adxl345bErrorCode_t errorCode =
        adxl345bInternalConvertRawValueToLSB(sensor, rawData, &intermediateLsb);
    if (errorCode != ADXL345B_NO_ERROR) {
        PRINT_DEBUG("conversion to LSB failed");
        return errorCode;
    }

    float intermediateGValue;
    errorCode = adxl345bInternalConvertLSBtoGValue(intermediateLsb, &intermediateGValue);
    if (errorCode != ADXL345B_NO_ERROR) {
        PRINT_DEBUG("conversion to G value failed");
        return errorCode;
    }

    *gValue = intermediateGValue;
    return ADXL345B_NO_ERROR;
}

static adxl345bErrorCode_t adxl345bInternalSetRegisterBits(adxl345bSensorConfiguration_t sensor,
                                                           adxl345bRegister_t adxlRegister,
                                                           uint8_t mask) {
    uint8_t currentSetting;
    adxl345bErrorCode_t errorCode = adxl345bInternalReadDataFromSensor(
        sensor, ADXL345B_REGISTER_DATA_FORMAT, &currentSetting, 1);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }
    currentSetting = currentSetting & ~mask;
    uint8_t newSetting = currentSetting & mask;

    errorCode =
        adxl345bWriteConfigurationToSensor(sensor, ADXL345B_REGISTER_DATA_FORMAT, newSetting);
    return errorCode;
}

static adxl345bErrorCode_t adxl345bInternalClearRegisterBits(adxl345bSensorConfiguration_t sensor,
                                                             adxl345bRegister_t adxlRegister,
                                                             uint8_t mask) {

    uint8_t currentSetting;
    adxl345bErrorCode_t errorCode = adxl345bInternalReadDataFromSensor(
        sensor, ADXL345B_REGISTER_DATA_FORMAT, &currentSetting, 1);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }
    currentSetting = currentSetting & ~mask;

    errorCode =
        adxl345bWriteConfigurationToSensor(sensor, ADXL345B_REGISTER_DATA_FORMAT, currentSetting);
    return errorCode;
}

static adxl345bErrorCode_t
adxl345bInternalDisableSelftestForce(adxl345bSensorConfiguration_t sensor) {
    return adxl345bInternalClearRegisterBits(sensor, ADXL345B_REGISTER_DATA_FORMAT,
                                             ADXL345B_BITMASK_SELF_TEST);
}

static adxl345bErrorCode_t
adxl345bInternalEnableSelftestForce(adxl345bSensorConfiguration_t sensor) {
    return adxl345bInternalSetRegisterBits(sensor, ADXL345B_REGISTER_DATA_FORMAT,
                                           ADXL345B_BITMASK_SELF_TEST);
}

static adxl345bErrorCode_t
adxl345bInternalWriteDefaultLowPowerConfiguration(adxl345bSensorConfiguration_t sensor) {
    PRINT_DEBUG("write default config to sensor");
    /* enable low power mode at 12.5Hz data output rate */
    adxl345bErrorCode_t errorCode = adxl345bWriteConfigurationToSensor(
        sensor, ADXL345B_REGISTER_BW_RATE, ADXL345B_BW_RATE_LPM_12_point_5);
    if (errorCode != ADXL345B_NO_ERROR) {
        PRINT_DEBUG("send ADXL345B_REGISTER_BW_RATE failed");
        PRINT_DEBUG("error code was %i", errorCode);
        return errorCode;
    }

    /* disable auto sleep, enable normal operation mode */
    errorCode = adxl345bWriteConfigurationToSensor(sensor, ADXL345B_REGISTER_POWER_CONTROL,
                                                   ADXL345B_BITMASK_MEASURE);
    if (errorCode != ADXL345B_NO_ERROR) {
        PRINT_DEBUG("send ADXL345B_REGISTER_POWER_CONTROL failed");
        PRINT_DEBUG("error code was %i", errorCode);
        return errorCode;
    }

    /* disable all interrupts */
    errorCode =
        adxl345bWriteConfigurationToSensor(sensor, ADXL345B_REGISTER_INTERRUPT_ENABLE, 0b00000000);
    if (errorCode != ADXL345B_NO_ERROR) {
        PRINT_DEBUG("send ADXL345B_REGISTER_INTERRUPT_ENABLE failed");
        PRINT_DEBUG("error code was %i", errorCode);
        return errorCode;
    }

    /* right adjusted storage, enable 10 bit 2G resolution */
    errorCode = adxl345bChangeMeasurementRange(sensor, ADXL345B_2G_RANGE);
    if (errorCode != ADXL345B_NO_ERROR) {
        PRINT_DEBUG("changing measurement range failed");
        PRINT_DEBUG("error code was %i", errorCode);
        return errorCode;
    }
    errorCode = adxl345bSetTenBitMode(sensor);

    return errorCode;
}

static adxl345bErrorCode_t adxl345bInternalCalculateCalibrationOffset(
    adxl345bSensorConfiguration_t sensor, int xMeasuredAtZeroG, int yMeasuredAtZeroG,
    int zMeasuredAtOneG, int8_t *xOffset, int8_t *yOffset, int8_t *zOffset) {
    int zExpectedAtZeroG = zMeasuredAtOneG - ADXL345B_IDEAL_SENSITIVITY_FULL_RES_3V3;

    uint8_t rawdata[6] = {0};
    adxl345bErrorCode_t errorCode = adxl345bGetSingleMeasurement(sensor, rawdata);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }

    int dataX, dataY, dataZ;
    errorCode = adxl345bInternalConvertRawValueToLSB(sensor, &rawdata[0], &dataX);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }
    errorCode = adxl345bInternalConvertRawValueToLSB(sensor, &rawdata[2], &dataY);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }
    errorCode = adxl345bInternalConvertRawValueToLSB(sensor, &rawdata[4], &dataZ);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }
    int xOffsetLSB = dataX - xMeasuredAtZeroG;
    int yOffsetLSB = dataY - yMeasuredAtZeroG;
    int zOffsetLSB = dataZ - zExpectedAtZeroG;

    float xyLSBtoOffsetScaleFactor, zLSBtoOffsetScaleFactor;
    adxl345bInternalCalculateOffsetRegisterScaleFactors(&xyLSBtoOffsetScaleFactor,
                                                        &zLSBtoOffsetScaleFactor);
    /* Because the offset register is additive,
     * the values are negated and rounded to the nearest LSB of the offset register */
    *xOffset = (int8_t)(-round((float)(xOffsetLSB) * (xyLSBtoOffsetScaleFactor)));
    *yOffset = (int8_t)(-round((float)(yOffsetLSB) * (xyLSBtoOffsetScaleFactor)));
    *zOffset = (int8_t)(-round((float)(zOffsetLSB) * (zLSBtoOffsetScaleFactor)));
    return errorCode;
}

static void adxl345bInternalCalculateOffsetRegisterScaleFactors(float *xyOffsetScaleFactor,
                                                                float *zOffsetScaleFactor) {
    float offsetRegisterResolution = 15.6;
    /* The z-axis sensitivity is unaffected by a change in supply voltage */
    float zSensitivityMgPerLsb = 1000.0f / ADXL345B_IDEAL_SENSITIVITY_FULL_RES_2V5;  // ~3.9
    float xySensitivityMgPerLsb = 1000.0f / ADXL345B_IDEAL_SENSITIVITY_FULL_RES_3V3; // ~3.77

    *zOffsetScaleFactor = zSensitivityMgPerLsb / offsetRegisterResolution;
    *xyOffsetScaleFactor = xySensitivityMgPerLsb / offsetRegisterResolution;
}
/* endregion HELPER FUNCTION IMPLEMENTATIONS */
