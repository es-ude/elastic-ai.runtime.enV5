#define SOURCE_FILE "src/adxl345b/adxl345b.c"

#include "Adxl345b.h"
#include "Adxl345bInternal.h"
#include "Adxl345bTypedefs.h"
#include "Common.h"
#include "I2c.h"
#include "pico/time.h"
#include <stdint.h>

/* region CONSTANTS */

static adxl345bI2cSensorConfiguration_t adxl345bI2cSensorConfiguration; /*!< i2c configuration for
                                                                         * the sensor */

static adxl345bRange_t adxl345bSelectedRange; /*!< measurement range configuration */

/* endregion */

/* region HEADER FUNCTION IMPLEMENTATIONS */

adxl345bErrorCode_t adxl345bInit(i2c_inst_t *i2cHost, adxl345bI2cSlaveAddress_t i2cAddress) {
    /* save i2c access for later usage */
    adxl345bI2cSensorConfiguration.i2c_host = i2cHost;
    adxl345bI2cSensorConfiguration.i2c_slave_address = i2cAddress;

    i2cInit(adxl345bI2cSensorConfiguration.i2c_host, (100 * 1000), 0, 1);

    /* sleep to make sure the sensor is fully initialized */
    sleep_ms(2);

    /* Check if sensor ADXL345B is on Bus by requesting serial number without
     * processing */
    uint8_t sizeOfCommandBuffer = 1;
    uint8_t commandBuffer[sizeOfCommandBuffer];
    commandBuffer[0] = ADXL345B_REGISTER_DEVICE_ID;

    /* if i2c returns error -> sensor not available on bus */
    i2cErrorCode_t i2CErrorCode = i2cWriteCommand(commandBuffer, sizeOfCommandBuffer,
                                                  adxl345bI2cSensorConfiguration.i2c_slave_address,
                                                  adxl345bI2cSensorConfiguration.i2c_host);
    if (i2CErrorCode != I2C_NO_ERROR) {
        return ADXL345B_INIT_ERROR;
    }

    adxl345bErrorCode_t errorCode = adxl345bInternalWriteDefaultLowPowerConfiguration();
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }

    return ADXL345B_NO_ERROR;
}

adxl345bErrorCode_t adxl345bWriteConfigurationToSensor(adxl345bRegister_t registerToWrite,
                                                       adxl345bConfiguration_t configuration) {
    uint8_t sizeOfCommandBuffer = 2;
    uint8_t commandBuffer[sizeOfCommandBuffer];
    commandBuffer[0] = registerToWrite;
    commandBuffer[1] = configuration;

    i2cErrorCode_t i2cErrorCode = i2cWriteCommand(commandBuffer, sizeOfCommandBuffer,
                                                  adxl345bI2cSensorConfiguration.i2c_slave_address,
                                                  adxl345bI2cSensorConfiguration.i2c_host);
    if (i2cErrorCode != I2C_NO_ERROR) {
        return ADXL345B_CONFIGURATION_ERROR;
    }

    return ADXL345B_NO_ERROR;
}

adxl345bErrorCode_t adxl345bChangeMeasurementRange(adxl345bRange_t newRange) {
    adxl345bSelectedRange = newRange;

    /* right adjusted storage, selected range settings for full resolution */
    adxl345bErrorCode_t errorCode = adxl345bWriteConfigurationToSensor(
        ADXL345B_REGISTER_DATA_FORMAT, adxl345bSelectedRange.settingForRange);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }

    return ADXL345B_NO_ERROR;
}

adxl345bErrorCode_t adxl345bReadSerialNumber(uint8_t *serialNumber) {
    uint8_t sizeOfResponseBuffer = 1;
    uint8_t responseBuffer[sizeOfResponseBuffer];

    adxl345bErrorCode_t adxl345bErrorCode = adxl345bInternalReadDataFromSensor(
        ADXL345B_REGISTER_DEVICE_ID, responseBuffer, sizeOfResponseBuffer);
    if (adxl345bErrorCode != ADXL345B_NO_ERROR) /* if i2c returns error -> sensor not available on
                                                   bus */
    {
        return adxl345bErrorCode;
    }

    *serialNumber = responseBuffer[0];

    return ADXL345B_NO_ERROR;
}

adxl345bErrorCode_t adxl345bReadMeasurements(float *xAxis, float *yAxis, float *zAxis) {
    adxl345bErrorCode_t errorCode;
    uint8_t interruptSources;
    uint8_t sizeOfResponseBuffer = 6;
    uint8_t responseBuffer[sizeOfResponseBuffer];

    /* check if data is ready */
    do {
        errorCode = adxl345bInternalReadDataFromSensor(ADXL345B_REGISTER_INTERRUPT_SOURCE,
                                                       &interruptSources, 1);
        if (errorCode != ADXL345B_NO_ERROR) {
            return errorCode;
        }
    } while (!(interruptSources & 0b10000000));

    errorCode = adxl345bInternalReadDataFromSensor(ADXL345B_REGISTER_DATA_X, responseBuffer,
                                                   sizeOfResponseBuffer);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }

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

adxl345bErrorCode_t adxl345bPerformSelfTest(int *deltaX, int *deltaY, int *deltaZ) {
    adxl345bErrorCode_t errorCode;

    /* standard mode, 100Hz */
    errorCode = adxl345bWriteConfigurationToSensor(ADXL345B_REGISTER_BW_RATE, 0b00001010);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }
    /* disable sleep/wakeup functions, start measurements */
    errorCode = adxl345bWriteConfigurationToSensor(ADXL345B_REGISTER_POWER_CONTROL, 0b00001000);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }
    /* right adjusted, 16G range */
    errorCode = adxl345bChangeMeasurementRange(ADXL345B_16G_RANGE);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }

    sleep_ms(2);

    /* collect 100 samples without force */
    int samplesWithoutForce[100][3];
    int counter = 0;
    while (counter < 100) {
        uint8_t interrupt_source;
        errorCode = adxl345bInternalReadDataFromSensor(ADXL345B_REGISTER_INTERRUPT_SOURCE,
                                                       &interrupt_source, 1);
        if (errorCode != ADXL345B_NO_ERROR) {
            return errorCode;
        }

        if (interrupt_source & 0b10000000) {
            uint8_t sizeOfResponseBuffer = 6;
            uint8_t responseBuffer[sizeOfResponseBuffer];
            errorCode = adxl345bInternalReadDataFromSensor(ADXL345B_REGISTER_DATA_X, responseBuffer,
                                                           sizeOfResponseBuffer);
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

            sleep_ms(20);
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
    errorCode = adxl345bWriteConfigurationToSensor(ADXL345B_REGISTER_DATA_FORMAT, 0b10001000);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }

    sleep_ms(2);

    /* collect 100 samples with force */
    int samplesWithForce[100][3];
    counter = 0;
    while (counter < 100) {
        uint8_t interrupt_source;
        errorCode = adxl345bInternalReadDataFromSensor(ADXL345B_REGISTER_INTERRUPT_SOURCE,
                                                       &interrupt_source, 1);
        if (errorCode != ADXL345B_NO_ERROR) {
            return errorCode;
        }

        if ((interrupt_source & 0b10000000) == 0b10000000) {
            uint8_t sizeOfResponseBuffer = 6;
            uint8_t responseBuffer[sizeOfResponseBuffer];

            errorCode = adxl345bInternalReadDataFromSensor(ADXL345B_REGISTER_DATA_X, responseBuffer,
                                                           sizeOfResponseBuffer);
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

            sleep_ms(20);
            counter++;
        }
    }

    /* calculate average of samples without force */
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
    adxl345bInternalWriteDefaultLowPowerConfiguration();

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

adxl345bErrorCode_t adxl345bRunSelfCalibration() {
    int deltaX, deltaY, deltaZ;
    adxl345bErrorCode_t errorCode = adxl345bPerformSelfTest(&deltaX, &deltaY, &deltaZ);
    if (errorCode == ADXL345B_NO_ERROR) {
        return ADXL345B_NO_ERROR;
    }
    if (errorCode != ADXL345B_SELF_TEST_FAILED_FOR_X &&
        errorCode != ADXL345B_SELF_TEST_FAILED_FOR_Y &&
        errorCode != ADXL345B_SELF_TEST_FAILED_FOR_Z) {
        return errorCode;
    }

    int8_t offsetX, offsetY, offsetZ;
    int maxDeltaX = 489, maxDeltaY = -46, maxDeltaZ = 791;
    int minDeltaX = 46, minDeltaY = -489, minDeltaZ = 69;
    offsetX = adxl345bInternalCalculateCalibrationOffset(deltaX, maxDeltaX, minDeltaX);
    offsetY = adxl345bInternalCalculateCalibrationOffset(deltaY, maxDeltaY, minDeltaY);
    offsetZ = adxl345bInternalCalculateCalibrationOffset(deltaZ, maxDeltaZ, minDeltaZ);

    errorCode = adxl345bWriteConfigurationToSensor(ADXL345B_OFFSET_X, offsetX);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }
    errorCode = adxl345bWriteConfigurationToSensor(ADXL345B_OFFSET_Y, offsetY);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }
    errorCode = adxl345bWriteConfigurationToSensor(ADXL345B_OFFSET_Z, offsetZ);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }

    return ADXL345B_NO_ERROR;
}

/* endregion */

/* region HELPER FUNCTION IMPLEMENTATIONS */

static adxl345bErrorCode_t adxl345bInternalReadDataFromSensor(adxl345bRegister_t registerToRead,
                                                              uint8_t *responseBuffer,
                                                              uint8_t sizeOfResponseBuffer) {
    uint8_t sizeOfCommandBuffer = 1;
    uint8_t commandBuffer[sizeOfCommandBuffer];
    commandBuffer[0] = registerToRead;

    PRINT_DEBUG("requesting data from sensor")
    i2cErrorCode_t i2cErrorCode = i2cWriteCommand(commandBuffer, sizeOfCommandBuffer,
                                                  adxl345bI2cSensorConfiguration.i2c_slave_address,
                                                  adxl345bI2cSensorConfiguration.i2c_host);
    if (i2cErrorCode != I2C_NO_ERROR) {
        PRINT_DEBUG("sending request failed")
        return ADXL345B_SEND_COMMAND_ERROR;
    }

    PRINT_DEBUG("receiving data from sensor")
    i2cErrorCode = i2cReadData(responseBuffer, sizeOfResponseBuffer,
                               adxl345bI2cSensorConfiguration.i2c_slave_address,
                               adxl345bI2cSensorConfiguration.i2c_host);
    if (i2cErrorCode != I2C_NO_ERROR) {
        PRINT_DEBUG("receiving data failed")
        return ADXL345B_RECEIVE_DATA_ERROR;
    }

    PRINT_DEBUG("successful received data from sensor")
    return ADXL345B_NO_ERROR;
}

static adxl345bErrorCode_t adxl345bInternalConvertRawValueToLSB(const uint8_t rawData[2],
                                                                int *lsbValue) {
    PRINT_DEBUG("convert raw data to LSB value")
    if (rawData[1] <= (adxl345bSelectedRange.msbMask >> 1)) {
        PRINT_DEBUG("entered positive case")
        uint16_t rawValue = ((uint16_t)(rawData[1] & (adxl345bSelectedRange.msbMask >> 1)) << 8) |
                            (uint16_t)rawData[0];
        *lsbValue = (int)rawValue;
    } else {
        PRINT_DEBUG("entered negative case")
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

    PRINT_DEBUG("conversion successful")
    return ADXL345B_NO_ERROR;
}

static adxl345bErrorCode_t adxl345bInternalConvertLSBtoGValue(int lsb, float *gValue) {
    PRINT_DEBUG("convert LSB value to G value")
    float realValue = (float)lsb * adxl345bSelectedRange.scaleFactor;
    *gValue = realValue;
    PRINT_DEBUG("LSB: %i, G value: %f", lsb, realValue)

    return ADXL345B_NO_ERROR;
}

static adxl345bErrorCode_t adxl345bInternalConvertRawValueToGValue(const uint8_t rawData[2],
                                                                   float *gValue) {
    PRINT_DEBUG("converting raw data to G value")
    int intermediateLsb;
    adxl345bErrorCode_t errorCode = adxl345bInternalConvertRawValueToLSB(rawData, &intermediateLsb);
    if (errorCode != ADXL345B_NO_ERROR) {
        PRINT_DEBUG("conversion to LSB failed")
        return errorCode;
    }

    float intermediateGValue;
    errorCode = adxl345bInternalConvertLSBtoGValue(intermediateLsb, &intermediateGValue);
    if (errorCode != ADXL345B_NO_ERROR) {
        PRINT_DEBUG("conversion to G value failed")
        return errorCode;
    }

    *gValue = intermediateGValue;
    return ADXL345B_NO_ERROR;
}

static adxl345bErrorCode_t adxl345bInternalWriteDefaultLowPowerConfiguration() {
    PRINT_DEBUG("write default config to sensor")
    /* enable low power mode at 12.5Hz data output rate */
    adxl345bErrorCode_t errorCode =
        adxl345bWriteConfigurationToSensor(ADXL345B_REGISTER_BW_RATE, 0b00010111);
    if (errorCode != ADXL345B_NO_ERROR) {
        PRINT_DEBUG("send ADXL345B_REGISTER_BW_RATE failed")
        PRINT_DEBUG("error code was %i", errorCode)
        return errorCode;
    }

    /* disable auto sleep, enable normal operation mode */
    errorCode = adxl345bWriteConfigurationToSensor(ADXL345B_REGISTER_POWER_CONTROL, 0b00001000);
    if (errorCode != ADXL345B_NO_ERROR) {
        PRINT_DEBUG("send ADXL345B_REGISTER_POWER_CONTROL failed")
        PRINT_DEBUG("error code was %i", errorCode)
        return errorCode;
    }

    /* disable all interrupts */
    errorCode = adxl345bWriteConfigurationToSensor(ADXL345B_REGISTER_INTERRUPT_ENABLE, 0b00000000);
    if (errorCode != ADXL345B_NO_ERROR) {
        PRINT_DEBUG("send ADXL345B_REGISTER_INTERRUPT_ENABLE failed")
        PRINT_DEBUG("error code was %i", errorCode)
        return errorCode;
    }

    /* right adjusted storage, enable 10 bit 2G resolution */
    errorCode = adxl345bChangeMeasurementRange(ADXL345B_2G_RANGE);
    if (errorCode != ADXL345B_NO_ERROR) {
        PRINT_DEBUG("changing measurement range failed")
        PRINT_DEBUG("error code was %i", errorCode)
        return errorCode;
    }

    return ADXL345B_NO_ERROR;
}

static int8_t adxl345bInternalCalculateCalibrationOffset(int measuredDelta, int maxValue,
                                                         int minValue) {
    PRINT_DEBUG("trying to calibrate offset")
    if (measuredDelta <= minValue) {
        PRINT_DEBUG("offset is %i", minValue - measuredDelta)
        return (int8_t)(minValue - measuredDelta);
    }
    if (measuredDelta >= maxValue) {
        PRINT_DEBUG("offset is %i", minValue - measuredDelta)
        return (int8_t)(maxValue - measuredDelta);
    }
    PRINT_DEBUG("offset is %i", 0)
    return 0;
}

/* endregion */
