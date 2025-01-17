#define SOURCE_FILE "ADXL345B-LIB"

#include "include/Adxl345b.h"
#include "Adxl345bInternal.h"
#include "Common.h"
#include "I2c.h"
#include "Sleep.h"
#include "Time.h"
#include "include/Adxl345bTypedefs.h"

/* region CONSTANTS */

/*! For more Information about Mask see Figure 49 on Page 32 of 36 in Datasheet  */
const static adxl345bRangeSetting_t adxl345b_2g_range = {0b00001000, 0b00000011, 0.0043f};
const static adxl345bRangeSetting_t adxl345b_4g_range = {0b00001001, 0b00000111, 0.0087f};
const static adxl345bRangeSetting_t adxl345b_8g_range = {0b00001010, 0b00001111, 0.0175f};
const static adxl345bRangeSetting_t adxl345b_16g_range = {0b00001011, 0b00111111, 0.0345f};

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

    /* if i2c returns error -> sensor not available on bus */
    i2cErrorCode_t i2CErrorCode = i2cWriteCommand(sensor.i2c_host, sensor.i2c_slave_address,
                                                  commandBuffer, sizeOfCommandBuffer);
    if (i2CErrorCode != I2C_NO_ERROR) {
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
        return ADXL345B_CONFIGURATION_ERROR;
    }

    return ADXL345B_NO_ERROR;
}

adxl345bErrorCode_t adxl345bSetFIFOMode(adxl345bSensorConfiguration_t sensor, uint8_t fifoMode,
                                        uint16_t samplesForTrigger) {
    uint8_t fifoConfiguration = (fifoMode & 0b11000000) | (samplesForTrigger & 0b00011111);
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

    /* right adjusted storage, selected range settings for full resolution */
    errorCode = adxl345bWriteConfigurationToSensor(sensor, ADXL345B_REGISTER_DATA_FORMAT,
                                                   adxl345bSelectedRange.settingForRange);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }
    return ADXL345B_NO_ERROR;
}

adxl345bErrorCode_t adxl345bReadSerialNumber(adxl345bSensorConfiguration_t sensor,
                                             uint8_t *serialNumber) {
    uint8_t sizeOfResponseBuffer = 1;
    uint8_t responseBuffer[sizeOfResponseBuffer];

    adxl345bErrorCode_t adxl345bErrorCode = adxl345bInternalReadDataFromSensor(
        sensor, ADXL345B_REGISTER_DEVICE_ID, responseBuffer, sizeOfResponseBuffer);
    // if i2c returns error -> sensor not available on bus
    if (adxl345bErrorCode != ADXL345B_NO_ERROR) {
        return adxl345bErrorCode;
    }

    *serialNumber = responseBuffer[0];

    return ADXL345B_NO_ERROR;
}

adxl345bErrorCode_t adxl345bGetSingleMeasurement(adxl345bSensorConfiguration_t sensor,
                                                 uint8_t *rawData) {
    adxl345bErrorCode_t errorCode;
    errorCode = adxl345bInternalCheckInterruptSource(sensor, 0b10000000); // check if data is ready
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
    uint8_t samplesInFifo = (fifoInformation & 0b00011111) + 1;

    if ((fifoInformation & 0b11000000) == ADXL345B_FIFOMODE_BYPASS) {
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
        /* update variables */
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

    /* read and set configuration*/
    uint8_t fifoInformation;
    errorCode =
        adxl345bInternalReadDataFromSensor(sensor, ADXL345B_FIFO_CONTROL, &fifoInformation, 1);
    if (errorCode != ADXL345B_NO_ERROR) {
        PRINT_DEBUG("read FIFO_CONTROL failed");
        return errorCode;
    }

    uint8_t samplesInFifo = (fifoInformation & 0b00011111) + 1;
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

adxl345bErrorCode_t adxl345bInternalCheckInterruptSource(adxl345bSensorConfiguration_t sensor,
                                                         uint8_t mask) {
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

adxl345bErrorCode_t adxl345bConvertDataXYZ(float *xAxis, float *yAxis, float *zAxis,
                                           uint8_t *responseBuffer) {
    adxl345bErrorCode_t errorCode;
    errorCode = adxl345bInternalConvertRawValueToGValue(&responseBuffer[0], xAxis);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }
    errorCode = adxl345bInternalConvertRawValueToGValue(&responseBuffer[2], yAxis);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }
    errorCode = adxl345bInternalConvertRawValueToGValue(&responseBuffer[4], zAxis);
    return errorCode;
}

adxl345bErrorCode_t adxl345bResetTrigger(adxl345bSensorConfiguration_t sensor,
                                         uint8_t fifoInformation) {
    adxl345bErrorCode_t errorCode;
    PRINT_DEBUG("reset trigger");
    errorCode = adxl345bSetFIFOMode(sensor, ADXL345B_FIFOMODE_BYPASS, fifoInformation & 0b00011111);
    if (errorCode != ADXL345B_NO_ERROR) {
        PRINT_DEBUG("reset trigger failed");
        return errorCode;
    }
    errorCode =
        adxl345bSetFIFOMode(sensor, ADXL345B_FIFOMODE_TRIGGER, fifoInformation & 0b00011111);
    if (errorCode != ADXL345B_NO_ERROR) {
        PRINT_DEBUG("reset trigger failed");
        return errorCode;
    }
    return errorCode;
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
        } while ((fifoStatus & 0b00111111) <
                 (remainder / 6)); // check entries until enough data available
        PRINT_DEBUG("start reading remaining samples");
    } else {
        errorCode = adxl345bInternalCheckInterruptSource(sensor, 0b00000010); // check watermark

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
    if ((fifoInformation & 0b11000000) == ADXL345B_FIFOMODE_TRIGGER) {
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

adxl345bErrorCode_t adxl345bPerformSelfTest(adxl345bSensorConfiguration_t sensor, int *deltaX,
                                            int *deltaY, int *deltaZ) {
    adxl345bErrorCode_t errorCode;

    /* standard mode, 100Hz */
    errorCode = adxl345bWriteConfigurationToSensor(sensor, ADXL345B_REGISTER_BW_RATE,
                                                   ADXL345B_BW_RATE_LPM_12_point_5);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }
    /* disable sleep/wakeup functions, start measurements */
    errorCode = adxl345bWriteConfigurationToSensor(sensor, ADXL345B_REGISTER_POWER_CONTROL,
                                                   ADXL345B_BW_RATE_25);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }
    /* right adjusted, 16G range */
    errorCode = adxl345bChangeMeasurementRange(sensor, ADXL345B_16G_RANGE);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }

    sleep_for_ms(2);

    /* collect 100 samples without force */
    int samplesWithoutForce[100][3];
    int counter = 0;
    while (counter < 100) {
        uint8_t interrupt_source;
        errorCode = adxl345bInternalReadDataFromSensor(sensor, ADXL345B_REGISTER_INTERRUPT_SOURCE,
                                                       &interrupt_source, 1);
        if (errorCode != ADXL345B_NO_ERROR) {
            return errorCode;
        }

        if (interrupt_source & 0b10000000) {
            uint8_t sizeOfResponseBuffer = 6;
            uint8_t responseBuffer[sizeOfResponseBuffer];
            errorCode = adxl345bInternalReadDataFromSensor(sensor, ADXL345B_REGISTER_DATA_X,
                                                           responseBuffer, sizeOfResponseBuffer);
            if (errorCode != ADXL345B_NO_ERROR) {
                return errorCode;
            }

            int dataX, dataY, dataZ;
            errorCode = adxl345bInternalConvertRawValueToLSB(&responseBuffer[0], &dataX);
            if (errorCode != ADXL345B_NO_ERROR) {
                return errorCode;
            }
            errorCode = adxl345bInternalConvertRawValueToLSB(&responseBuffer[2], &dataY);
            if (errorCode != ADXL345B_NO_ERROR) {
                return errorCode;
            }
            errorCode = adxl345bInternalConvertRawValueToLSB(&responseBuffer[4], &dataZ);
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
    for (int index = 0; index < 100; index++) {
        sumSamplesWithoutForceX += samplesWithoutForce[index][0];
        sumSamplesWithoutForceY += samplesWithoutForce[index][1];
        sumSamplesWithoutForceZ += samplesWithoutForce[index][2];
    }
    int avgSampleWithoutForceX = sumSamplesWithoutForceX / 100;
    int avgSampleWithoutForceY = sumSamplesWithoutForceY / 100;
    int avgSampleWithoutForceZ = sumSamplesWithoutForceZ / 100;

    /* enable self test force */
    errorCode =
        adxl345bWriteConfigurationToSensor(sensor, ADXL345B_REGISTER_DATA_FORMAT, 0b10001000);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }

    sleep_for_ms(2);

    /* collect 100 samples with force */
    int samplesWithForce[100][3];
    counter = 0;
    while (counter < 100) {
        uint8_t interrupt_source;
        errorCode = adxl345bInternalReadDataFromSensor(sensor, ADXL345B_REGISTER_INTERRUPT_SOURCE,
                                                       &interrupt_source, 1);
        if (errorCode != ADXL345B_NO_ERROR) {
            return errorCode;
        }

        if ((interrupt_source & 0b10000000) == 0b10000000) {
            uint8_t sizeOfResponseBuffer = 6;
            uint8_t responseBuffer[sizeOfResponseBuffer];

            errorCode = adxl345bInternalReadDataFromSensor(sensor, ADXL345B_REGISTER_DATA_X,
                                                           responseBuffer, sizeOfResponseBuffer);
            if (errorCode != ADXL345B_NO_ERROR) {
                return errorCode;
            }

            int dataX, dataY, dataZ;
            errorCode = adxl345bInternalConvertRawValueToLSB(&responseBuffer[0], &dataX);
            if (errorCode != ADXL345B_NO_ERROR) {
                return errorCode;
            }
            errorCode = adxl345bInternalConvertRawValueToLSB(&responseBuffer[2], &dataY);
            if (errorCode != ADXL345B_NO_ERROR) {
                return errorCode;
            }
            errorCode = adxl345bInternalConvertRawValueToLSB(&responseBuffer[4], &dataZ);
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
    for (int index = 0; index < 100; index++) {
        sumSamplesWithForceX += samplesWithForce[index][0];
        sumSamplesWithForceY += samplesWithForce[index][1];
        sumSamplesWithForceZ += samplesWithForce[index][2];
    }
    int avgSampleWithForceX = sumSamplesWithForceX / 100;
    int avgSampleWithForceY = sumSamplesWithForceY / 100;
    int avgSampleWithForceZ = sumSamplesWithForceZ / 100;

    // return to default operation mode
    adxl345bInternalWriteDefaultLowPowerConfiguration(sensor);

    /* compare average to datasheet */
    int deltaOfAverageX = avgSampleWithForceX - avgSampleWithoutForceX;
    *deltaX = deltaOfAverageX;

    int deltaOfAverageY = avgSampleWithForceY - avgSampleWithoutForceY;
    *deltaY = deltaOfAverageY;

    int deltaOfAverageZ = avgSampleWithForceZ - avgSampleWithoutForceZ;
    *deltaZ = deltaOfAverageZ;

    int maxDeltaX = 489, maxDeltaY = -46, maxDeltaZ = 791;
    int minDeltaX = 46, minDeltaY = -489, minDeltaZ = 69;
    if (!(minDeltaX <= deltaOfAverageX && deltaOfAverageX <= maxDeltaX)) {
        return ADXL345B_SELF_TEST_FAILED_FOR_X;
    }

    if (!(minDeltaY <= deltaOfAverageY && deltaOfAverageY <= maxDeltaY)) {
        return ADXL345B_SELF_TEST_FAILED_FOR_Y;
    }

    if (!(minDeltaZ <= deltaOfAverageZ && deltaOfAverageZ <= maxDeltaZ)) {
        return ADXL345B_SELF_TEST_FAILED_FOR_Z;
    }

    return ADXL345B_NO_ERROR;
}

adxl345bErrorCode_t adxl345bRunSelfCalibration(adxl345bSensorConfiguration_t sensor) {
    int deltaX, deltaY, deltaZ;
    adxl345bErrorCode_t errorCode = adxl345bPerformSelfTest(sensor, &deltaX, &deltaY, &deltaZ);
    if (errorCode == ADXL345B_NO_ERROR) {
        return ADXL345B_NO_ERROR;
    }
    if (errorCode != ADXL345B_SELF_TEST_FAILED_FOR_X &&
        errorCode != ADXL345B_SELF_TEST_FAILED_FOR_Y &&
        errorCode != ADXL345B_SELF_TEST_FAILED_FOR_Z) {
        return errorCode;
    }

    int8_t offsetX, offsetY, offsetZ;
    // TODO: Woher kommen diese Werte?
    int maxDeltaX = 489, maxDeltaY = -46, maxDeltaZ = 791;
    int minDeltaX = 46, minDeltaY = -489, minDeltaZ = 69;
    offsetX = adxl345bInternalCalculateCalibrationOffset(deltaX, maxDeltaX, minDeltaX);
    offsetY = adxl345bInternalCalculateCalibrationOffset(deltaY, maxDeltaY, minDeltaY);
    offsetZ = adxl345bInternalCalculateCalibrationOffset(deltaZ, maxDeltaZ, minDeltaZ);

    errorCode = adxl345bWriteConfigurationToSensor(sensor, ADXL345B_OFFSET_X, offsetX);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }
    errorCode = adxl345bWriteConfigurationToSensor(sensor, ADXL345B_OFFSET_Y, offsetY);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }
    errorCode = adxl345bWriteConfigurationToSensor(sensor, ADXL345B_OFFSET_Z, offsetZ);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }

    return ADXL345B_NO_ERROR;
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

static adxl345bErrorCode_t adxl345bInternalSetSelectedRange(adxl345bRange_t range) {
    switch (range) {
    case ADXL345B_2G_RANGE:
        adxl345bSelectedRange = adxl345b_2g_range;
        break;
    case ADXL345B_4G_RANGE:
        adxl345bSelectedRange = adxl345b_4g_range;
        break;
    case ADXL345B_8G_RANGE:
        adxl345bSelectedRange = adxl345b_8g_range;
        break;
    case ADXL345B_16G_RANGE:
        adxl345bSelectedRange = adxl345b_16g_range;
        break;
    default:
        adxl345bSelectedRange = adxl345b_2g_range;
        return ADXL345B_RANGE_ERROR;
    }
    return ADXL345B_NO_ERROR;
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

static adxl345bErrorCode_t adxl345bInternalConvertRawValueToLSB(const uint8_t rawData[2],
                                                                int *lsbValue) {
    PRINT_DEBUG("convert raw data to LSB value");
    if (rawData[1] <= (adxl345bSelectedRange.msbMask >> 1)) {
        PRINT_DEBUG("entered positive case");
        uint16_t rawValue = ((uint16_t)(rawData[1] & (adxl345bSelectedRange.msbMask >> 1)) << 8) |
                            (uint16_t)rawData[0];
        *lsbValue = (int)rawValue;
    } else {
        PRINT_DEBUG("entered negative case");
        /* manual translation needed
         *
         * 1. revert two complement: number minus 1 and flip bits
         * 2. convert to int and multiply with -1
         */
        uint16_t rawValue = ((uint16_t)(rawData[1] & (adxl345bSelectedRange.msbMask >> 1)) << 8) |
                            (uint16_t)rawData[0];
        rawValue = (rawValue - 0x0001) ^ ((adxl345bSelectedRange.msbMask >> 1) << 8 | 0x00FF);
        *lsbValue = (-1) * (int)rawValue;
    }

    PRINT_DEBUG("conversion successful");
    return ADXL345B_NO_ERROR;
}

static adxl345bErrorCode_t adxl345bInternalConvertLSBtoGValue(int lsb, float *gValue) {
    PRINT_DEBUG("convert LSB value to G value");
    float realValue = (float)lsb * adxl345bSelectedRange.scaleFactor;
    *gValue = realValue;
    PRINT_DEBUG("LSB: %i, G value: %f", lsb, realValue);

    return ADXL345B_NO_ERROR;
}

static adxl345bErrorCode_t adxl345bInternalConvertRawValueToGValue(const uint8_t rawData[2],
                                                                   float *gValue) {
    PRINT_DEBUG("converting raw data to G value");
    int intermediateLsb;
    adxl345bErrorCode_t errorCode = adxl345bInternalConvertRawValueToLSB(rawData, &intermediateLsb);
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
    errorCode =
        adxl345bWriteConfigurationToSensor(sensor, ADXL345B_REGISTER_POWER_CONTROL, 0b00001000);
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

    return ADXL345B_NO_ERROR;
}

static int8_t adxl345bInternalCalculateCalibrationOffset(int measuredDelta, int maxValue,
                                                         int minValue) {
    PRINT_DEBUG("trying to calibrate offset");
    if (measuredDelta <= minValue) {
        PRINT_DEBUG("offset is %i", minValue - measuredDelta);
        return (int8_t)(minValue - measuredDelta);
    }
    if (measuredDelta >= maxValue) {
        PRINT_DEBUG("offset is %i", minValue - measuredDelta);
        return (int8_t)(maxValue - measuredDelta);
    }
    PRINT_DEBUG("offset is %i", 0);
    return 0;
}

/* endregion HELPER FUNCTION IMPLEMENTATIONS */
