#define SOURCE_FILE "ADXL345B-LIB"

#include "Adxl345b.h"
#include "Adxl345bInternal.h"
#include "Adxl345bTypedefs.h"
#include "Common.h"
#include "I2c.h"
#include "Sleep.h"

/* region CONSTANTS */

/*!TODO: add documentation why mask ? */
const static adxl345bRangeSetting_t adxl345b_2g_range = {0b00001000, 0b00000011, 0.0043f};
const static adxl345bRangeSetting_t adxl345b_4g_range = {0b00001001, 0b00000111, 0.0087f};
const static adxl345bRangeSetting_t adxl345b_8g_range = {0b00001010, 0b00001111, 0.0175f};
/*!TODO: add documentation why mask not 0b00011111? */
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

/* NOTE:
 * In stream mode, data from measurements of the x-, y-, and z-axes
are stored in FIFO. When the number of samples in FIFO equals
the level specified in the samples bits of the ADXL345B_FIFO_CONTROL
(Address 0x38), the watermark interrupt is set. FIFO continues
accumulating samples and holds the latest 32 samples from meas-
urements of the x-, y-, and z-axes, discarding older data as new
data arrives. The watermark interrupt continues occurring until the
number of samples in FIFO is less than the value stored in the
samples bits of the ADXL345B_FIFO_CONTROL register.
======================TRIGGER MODE==================================
In trigger mode, FIFO accumulates samples, holding the latest 32
samples from measurements of the x-, y-, and z-axes. After a
trigger event occurs and an interrupt is sent to the INT1 or INT2
pin (determined by the trigger bit in the FIFO_CTL register), FIFO
keeps the last n samples (where n is the value specified by the
samples bits in the FIFO_CTL register) and then operates in FIFO
mode, collecting new samples only when FIFO is not full. A delay of
at least 5 μs should be present between the trigger event occurring
and the start of reading data from the FIFO to allow the FIFO to
discard and retain the necessary samples. Additional trigger events
cannot be recognized until the trigger mode is reset. To reset the
trigger mode, set the device to bypass mode and then set the
device back to trigger mode. Note that the FIFO data should be
read first because placing the device into bypass mode clears
FIFO.*/
adxl345bErrorCode_t adxl345bEnableStreamMode(adxl345bSensorConfiguration_t sensor){
    return ADXL345B_NO_ERROR;
}

static adxl345bErrorCode_t adxl345bSetSelectedRange( adxl345bRange_t range ){
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
return ADXL345B_NO_ERROR;}

adxl345bErrorCode_t adxl345bChangeMeasurementRange(adxl345bSensorConfiguration_t sensor, adxl345bRange_t newRange ) {
    adxl345bErrorCode_t errorCode = adxl345bSetSelectedRange(newRange);
    if (errorCode != ADXL345B_NO_ERROR) {return errorCode;}

    /* right adjusted storage, selected range settings for full resolution */
        errorCode = adxl345bWriteConfigurationToSensor(sensor,
            ADXL345B_REGISTER_DATA_FORMAT, adxl345bSelectedRange.settingForRange
        );
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

//TODO:
//& umbenennen zu ReadMeasurement (singular!)oder "oneShot", die andere dann ReadMeasurementsInStream    ...
// anderer Layer dann 2 fuer InStream  dann fuer Intervall die dann andere per Anzahl
adxl345bErrorCode_t adxl345bReadMeasurementOneShot(adxl345bSensorConfiguration_t sensor, float *xAxis, float *yAxis, float *zAxis) {
    adxl345bErrorCode_t errorCode;
    volatile uint8_t interruptSources;
    uint8_t sizeOfResponseBuffer = 6;
    uint8_t responseBuffer[sizeOfResponseBuffer];

    do {
        errorCode = adxl345bInternalReadDataFromSensor(sensor, ADXL345B_REGISTER_INTERRUPT_SOURCE,
                                                       &interruptSources, 1);
        if (errorCode != ADXL345B_NO_ERROR) {
            return errorCode;
        }
    } while (!(interruptSources & 0b10000000)); // check if data is ready


    /*Register 0x32 to Register 0x37—DATAX0,
DATAX1, DATAY0, DATAY1, DATAZ0, DATAZ1
(Read Only)
These six bytes (Register 0x32 to Register 0x37 'sizeOfResponseBuffer' hold the output data for each axis.
Register 0x32 and 0x33 hold the output data for the x-axis,
Register 0x34 and 0x35 hold the output data for the y-axis,
Register 0x36 and 0x37 hold the output data for the z-axis.
The output data is twos complement, with DATAx0 as the least significant byte
and DATAx1 as the most significant byte, where x represent X,
Y, or Z. The DATA_FORMAT register (Address 0x31) controls the
format of the data.
It is recommended that a multiple-byte read of all
registers be performed to prevent a change in data between reads
of sequential registers

    !!!!wir wollen aus dem FIFO_CTL auslesen und nicht aus denX,y,z

    hier 2 funktionen draus machen: readxyz und convertxyz*/

    errorCode = adxl345bInternalReadDataFromSensor(sensor, ADXL345B_REGISTER_DATA_X, responseBuffer,
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

adxl345bErrorCode_t adxl345bPerformSelfTest(adxl345bSensorConfiguration_t sensor, int *deltaX,
                                            int *deltaY, int *deltaZ) {
    adxl345bErrorCode_t errorCode;

    /* standard mode, 100Hz */
    errorCode = adxl345bWriteConfigurationToSensor(sensor, ADXL345B_REGISTER_BW_RATE, ADXL345B_BW_RATE_LPM_12_point_5);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }
    /* disable sleep/wakeup functions, start measurements */
    errorCode =
        adxl345bWriteConfigurationToSensor(sensor, ADXL345B_REGISTER_POWER_CONTROL, ADXL345B_BW_RATE_25);
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
    //TODO: Woher kommen diese Werte?
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

//TODO: configuration zu Typedefs hinzufuegen!!! WIE?
static adxl345bErrorCode_t
adxl345bInternalWriteDefaultLowPowerConfiguration(adxl345bSensorConfiguration_t sensor) {
    PRINT_DEBUG("write default config to sensor");
    /* enable low power mode at 12.5Hz data output rate */
    adxl345bErrorCode_t errorCode =
        adxl345bWriteConfigurationToSensor(sensor, ADXL345B_REGISTER_BW_RATE, ADXL345B_BW_RATE_LPM_12_point_5);
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
