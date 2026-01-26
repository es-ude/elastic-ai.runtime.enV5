#define SOURCE_FILE "ADXL345B-LIB"

#include "include/eai/sensor/Adxl345b.h"
#include "Adxl345bInternal.h"
#include "eai/Common.h"
#include "eai/hal/I2c.h"
#include "eai/hal/Sleep.h"
#include "include/eai/sensor/Adxl345bTypedefs.h"

// #############################################################################
// Sensor Configuration
// #############################################################################

adxl345bErrorCode_t adxl345bInit(adxl345bSensorConfiguration_t *sensor) {
    // Check if sensor ADXL345B is on Bus by requesting serial number without processing
    uint8_t sizeOfCommandBuffer = 1;
    uint8_t commandBuffer[sizeOfCommandBuffer];
    commandBuffer[0] = ADXL345B_REGISTER_DEVID;
    i2cErrorCode_t i2cErrorCode = i2cWriteCommand(sensor->i2c_host, sensor->i2c_slave_address,
                                                  commandBuffer, sizeOfCommandBuffer);
    if (i2cErrorCode != I2C_NO_ERROR) {
        PRINT("sensor not available on bus; i2c_ERROR: %02X", i2cErrorCode);
        return ADXL345B_INIT_ERROR;
    }

    return adxl345bInternalWriteDefaultConfiguration(sensor);
}

adxl345bErrorCode_t adxl345bReadConfigurationFromSensor(adxl345bSensorConfiguration_t *sensor,
                                                        adxl345bRegister_t targetRegister,
                                                        uint8_t *config) {
    i2cErrorCode_t i2cErrorCode;

    uint8_t commandBuffer[1];
    commandBuffer[0] = targetRegister;

    i2cErrorCode = i2cWriteCommand(sensor->i2c_host, sensor->i2c_slave_address, commandBuffer, 1);
    if (i2cErrorCode != I2C_NO_ERROR) {
        PRINT("sensor not available on bus; i2c_ERROR: %02X", i2cErrorCode);
        return ADXL345B_SEND_COMMAND_ERROR;
    }

    i2cErrorCode = i2cReadData(sensor->i2c_host, sensor->i2c_slave_address, config, 1);
    if (i2cErrorCode != I2C_NO_ERROR) {
        PRINT("sensor not available on bus; i2c_ERROR: %02X", i2cErrorCode);
        return ADXL345B_RECEIVE_DATA_ERROR;
    }

    return ADXL345B_NO_ERROR;
}

adxl345bErrorCode_t adxl345bWriteConfigurationToSensor(adxl345bSensorConfiguration_t *sensor,
                                                       adxl345bRegister_t targetRegister,
                                                       adxl345bConfig_t config) {
    uint8_t sizeOfCommandBuffer = 2;
    uint8_t commandBuffer[sizeOfCommandBuffer];
    commandBuffer[0] = targetRegister;
    commandBuffer[1] = config;

    i2cErrorCode_t i2cErrorCode;
    i2cErrorCode = i2cWriteCommand(sensor->i2c_host, sensor->i2c_slave_address, commandBuffer,
                                   sizeOfCommandBuffer);
    if (i2cErrorCode != I2C_NO_ERROR) {
        PRINT("sensor not available on bus; i2c_ERROR: %02X", i2cErrorCode);
        return ADXL345B_CONFIGURATION_ERROR;
    }

    return ADXL345B_NO_ERROR;
}

adxl345bErrorCode_t adxl345bSetFIFOMode(adxl345bSensorConfiguration_t *sensor,
                                        adxl345bFifoMode_t fifoMode, uint8_t threshold) {
    uint8_t fifoConfiguration = fifoMode | (threshold & 0b00011111);
    return adxl345bWriteConfigurationToSensor(sensor, ADXL345B_FIFO_CTL, fifoConfiguration);
}

adxl345bErrorCode_t adxl345bActivateMeasurementMode(adxl345bSensorConfiguration_t *sensor) {
    adxl345bErrorCode_t errorCode;

    uint8_t power_ctrl_register;
    errorCode = adxl345bReadConfigurationFromSensor(sensor, ADXL345B_REGISTER_POWER_CTL,
                                                    &power_ctrl_register);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }

    return adxl345bWriteConfigurationToSensor(sensor, ADXL345B_REGISTER_POWER_CTL,
                                              (power_ctrl_register | ADXL345B_POWER_CTL_MEASURE));
}

adxl345bErrorCode_t adxl345bDeactivateMeasurementMode(adxl345bSensorConfiguration_t *sensor) {
    adxl345bErrorCode_t errorCode;

    uint8_t power_ctrl_register;
    errorCode = adxl345bReadConfigurationFromSensor(sensor, ADXL345B_REGISTER_POWER_CTL,
                                                    &power_ctrl_register);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }

    return adxl345bWriteConfigurationToSensor(sensor, ADXL345B_REGISTER_POWER_CTL,
                                              (power_ctrl_register & ~ADXL345B_POWER_CTL_MEASURE));
}

adxl345bErrorCode_t adxl345bSetFullResolutionMode(adxl345bSensorConfiguration_t *sensor) {
    adxl345bErrorCode_t errorCode;

    uint8_t data_frame_register;
    errorCode = adxl345bReadConfigurationFromSensor(sensor, ADXL345B_REGISTER_DATA_FORMAT,
                                                    &data_frame_register);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }

    errorCode =
        adxl345bWriteConfigurationToSensor(sensor, ADXL345B_REGISTER_DATA_FORMAT,
                                           (data_frame_register | ADXL345B_DATA_FORMAT_FULL_RES));
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }

    sensor->full_res_mode = true;
    return ADXL345B_NO_ERROR;
}

adxl345bErrorCode_t adxl345bSetTenBitMode(adxl345bSensorConfiguration_t *sensor) {
    adxl345bErrorCode_t errorCode;

    uint8_t data_frame_register;
    errorCode = adxl345bReadConfigurationFromSensor(sensor, ADXL345B_REGISTER_DATA_FORMAT,
                                                    &data_frame_register);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }

    errorCode =
        adxl345bWriteConfigurationToSensor(sensor, ADXL345B_REGISTER_DATA_FORMAT,
                                           (data_frame_register | ADXL345B_DATA_FORMAT_FULL_RES));
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }

    sensor->full_res_mode = false;
    return ADXL345B_NO_ERROR;
}

adxl345bErrorCode_t adxl345bChangeMeasurementRange(adxl345bSensorConfiguration_t *sensor,
                                                   adxl345bRange_t rangeToUse) {
    adxl345bErrorCode_t errorCode;

    switch (rangeToUse) {
    case ADXL345B_2G_RANGE:
    case ADXL345B_4G_RANGE:
    case ADXL345B_8G_RANGE:
    case ADXL345B_16G_RANGE:
        break;
    default:
        return ADXL345B_RANGE_ERROR;
    }

    uint8_t data_format_register;
    errorCode = adxl345bReadConfigurationFromSensor(sensor, ADXL345B_REGISTER_DATA_FORMAT,
                                                    &data_format_register);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }

    errorCode = adxl345bWriteConfigurationToSensor(sensor, ADXL345B_REGISTER_DATA_FORMAT,
                                                   (data_format_register | rangeToUse));
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }

    sensor->measurement_range = rangeToUse;
    return ADXL345B_NO_ERROR;
}

// #############################################################################
// Self-Test and Calibration
// #############################################################################

adxl345bErrorCode_t adxl345bReadSerialNumber(adxl345bSensorConfiguration_t *sensor,
                                             uint8_t *serialNumber) {
    return adxl345bReadConfigurationFromSensor(sensor, ADXL345B_REGISTER_DEVID, serialNumber);
}

adxl345bErrorCode_t adxl345bPerformSelfTest(adxl345bSensorConfiguration_t *sensor) {
    // FIXME: test fails even if values seem to be correct!
    adxl345bErrorCode_t errorCode;

    // configure sensor
    errorCode = adxl345bInternalWriteDefaultConfiguration(sensor);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }
    errorCode =
        adxl345bWriteConfigurationToSensor(sensor, ADXL345B_REGISTER_BW_RATE, ADXL345B_BW_RATE_100);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }
    errorCode = adxl345bChangeMeasurementRange(sensor, ADXL345B_8G_RANGE);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }
    errorCode = adxl345bSetTenBitMode(sensor);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }
    errorCode = adxl345bSetFIFOMode(sensor, ADXL345B_FIFOMODE_FIFO, 10);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }

    // declare data buffer for collected samples
    int numberOfSamples = 10;
    adxl345bRawData_t sampleBuffer[numberOfSamples];

    // get samples **with** self-test force from sensor
    errorCode = adxl345bInternalEnableSelfTestForce(sensor);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }
    sleep_for_ms(100);
    errorCode = adxl345bActivateMeasurementMode(sensor);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }
    sleep_for_ms(150);
    errorCode = adxl345bDeactivateMeasurementMode(sensor);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }
    errorCode = adxl345bInternalDisableSelfTestForce(sensor);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }
    sleep_for_ms(100);
    for (size_t sampleIndex = 0; sampleIndex < numberOfSamples; sampleIndex++) {
        errorCode = adxl345bGetSingleMeasurement(sensor, &(sampleBuffer[sampleIndex]));
        if (errorCode != ADXL345B_NO_ERROR) {
            return errorCode;
        }
    }
    errorCode = adxl345bInternalClearFifoBuffer(sensor);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }

    // convert raw data to lsb values and calculate average
    int sumSamplesWithForceX = 0;
    int sumSamplesWithForceY = 0;
    int sumSamplesWithForceZ = 0;
    for (size_t sampleIndex = 0; sampleIndex < numberOfSamples; sampleIndex++) {
        sumSamplesWithForceX += adxl345bInternalConvertRawDataToLsbValue(
            sampleBuffer[sampleIndex].measurement_range, sampleBuffer[sampleIndex].full_res_mode,
            &(sampleBuffer[sampleIndex].rawData[0]));
        sumSamplesWithForceY += adxl345bInternalConvertRawDataToLsbValue(
            sampleBuffer[sampleIndex].measurement_range, sampleBuffer[sampleIndex].full_res_mode,
            &(sampleBuffer[sampleIndex].rawData[2]));
        sumSamplesWithForceZ += adxl345bInternalConvertRawDataToLsbValue(
            sampleBuffer[sampleIndex].measurement_range, sampleBuffer[sampleIndex].full_res_mode,
            &(sampleBuffer[sampleIndex].rawData[4]));
    }
    int avgSampleWithForceX = sumSamplesWithForceX / numberOfSamples;
    int avgSampleWithForceY = sumSamplesWithForceY / numberOfSamples;
    int avgSampleWithForceZ = sumSamplesWithForceZ / numberOfSamples;

    // get samples **without** self-test force from sensor
    errorCode = adxl345bActivateMeasurementMode(sensor);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }
    sleep_for_ms(150);
    errorCode = adxl345bDeactivateMeasurementMode(sensor);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }
    for (size_t sampleIndex = 0; sampleIndex < numberOfSamples; sampleIndex++) {
        errorCode = adxl345bGetSingleMeasurement(sensor, &(sampleBuffer[sampleIndex]));
        if (errorCode != ADXL345B_NO_ERROR) {
            return errorCode;
        }
    }
    errorCode = adxl345bInternalClearFifoBuffer(sensor);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }

    // convert raw data to lsb values and calculate average
    int sumSamplesWithoutForceX = 0;
    int sumSamplesWithoutForceY = 0;
    int sumSamplesWithoutForceZ = 0;
    for (size_t sampleIndex = 0; sampleIndex < numberOfSamples; sampleIndex++) {
        sumSamplesWithoutForceX += adxl345bInternalConvertRawDataToLsbValue(
            sampleBuffer[sampleIndex].measurement_range, sampleBuffer[sampleIndex].full_res_mode,
            &(sampleBuffer[sampleIndex].rawData[0]));
        sumSamplesWithoutForceY += adxl345bInternalConvertRawDataToLsbValue(
            sampleBuffer[sampleIndex].measurement_range, sampleBuffer[sampleIndex].full_res_mode,
            &(sampleBuffer[sampleIndex].rawData[2]));
        sumSamplesWithoutForceZ += adxl345bInternalConvertRawDataToLsbValue(
            sampleBuffer[sampleIndex].measurement_range, sampleBuffer[sampleIndex].full_res_mode,
            &(sampleBuffer[sampleIndex].rawData[4]));
    }
    int avgSampleWithoutForceX = sumSamplesWithoutForceX / numberOfSamples;
    int avgSampleWithoutForceY = sumSamplesWithoutForceY / numberOfSamples;
    int avgSampleWithoutForceZ = sumSamplesWithoutForceZ / numberOfSamples;

    // calculate difference of averages
    int deltaOfAverageX = avgSampleWithForceX - avgSampleWithoutForceX;
    int deltaOfAverageY = avgSampleWithForceY - avgSampleWithoutForceY;
    int deltaOfAverageZ = avgSampleWithForceZ - avgSampleWithoutForceZ;

    /* Self-Test Output in LSB (See Table 14 and 15)
     * scale factor for 3.3V: X/Y axis 1.77 and Z axis 1.47
     * LSB reference for 8G 10 bit resolution:
     * -> X_min   12 LSB * 1.77 =   21.24 LSB ≈   21 LSB
     * -> X_max  135 LSB * 1.77 =  238.95 LSB ≈  239 LSB
     * -> Y_min -135 LSB * 1.77 = −238.95 LSB ≈ -238 LSB
     * -> Y_max  -12 LSB * 1.77 =  -21.24 LSB ≈  -22 LSB
     * -> Z_min   19 LSB * 1.47 =   27.93 LSB ≈   27 LSB
     * -> Z_max  219 LSB * 1.47 =  321.93 LSB ≈  322 LSB
     */
    int16_t minDeltaX = 21, maxDeltaX = 239;
    int16_t minDeltaY = -238, maxDeltaY = -22;
    int16_t minDeltaZ = 27, maxDeltaZ = 322;

    PRINT_DEBUG("wiht force X: %i", avgSampleWithForceX);
    PRINT_DEBUG("wiht force Y: %i", avgSampleWithForceY);
    PRINT_DEBUG("wiht force Z: %i", avgSampleWithForceZ);
    PRINT_DEBUG("wihtout force X: %i", avgSampleWithoutForceX);
    PRINT_DEBUG("wihtout force Y: %i", avgSampleWithoutForceY);
    PRINT_DEBUG("wihtout force Z: %i", avgSampleWithoutForceZ);

    bool failed = false;
    if (!(minDeltaX <= deltaOfAverageX && deltaOfAverageX <= maxDeltaX)) {
        PRINT_DEBUG("delta X: %i", deltaOfAverageX);
        failed = true;
    }
    if (!(minDeltaY <= deltaOfAverageY && deltaOfAverageY <= maxDeltaY)) {
        PRINT_DEBUG("delta Y: %i", deltaOfAverageY);
        failed = true;
    }
    if (!(minDeltaZ <= deltaOfAverageZ && deltaOfAverageZ <= maxDeltaZ)) {
        PRINT_DEBUG("delta Z: %i", deltaOfAverageZ);
        failed = true;
    }

    if (failed) {
        return ADXL345B_SELF_TEST_FAILED;
    }
    return adxl345bInternalWriteDefaultConfiguration(sensor);
}

adxl345bErrorCode_t adxl345bPerformSelfCalibration(adxl345bSensorConfiguration_t *sensor) {
    adxl345bErrorCode_t errorCode;

    // configure sensor
    errorCode = adxl345bInternalWriteDefaultConfiguration(sensor);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }
    errorCode =
        adxl345bWriteConfigurationToSensor(sensor, ADXL345B_REGISTER_BW_RATE, ADXL345B_BW_RATE_100);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }
    errorCode = adxl345bChangeMeasurementRange(sensor, ADXL345B_8G_RANGE);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }
    errorCode = adxl345bSetFullResolutionMode(sensor);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }
    errorCode = adxl345bSetFIFOMode(sensor, ADXL345B_FIFOMODE_FIFO, 10);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }

    // reset offset register
    errorCode = adxl345bWriteConfigurationToSensor(sensor, ADXL345B_REGISTER_OFSX, 0);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }
    errorCode = adxl345bWriteConfigurationToSensor(sensor, ADXL345B_REGISTER_OFSY, 0);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }
    errorCode = adxl345bWriteConfigurationToSensor(sensor, ADXL345B_REGISTER_OFSZ, 0);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }

    // declare data buffer for collected samples
    int numberOfSamples = 10;
    adxl345bRawData_t sampleBuffer[numberOfSamples];

    // get samples from sensor
    errorCode = adxl345bActivateMeasurementMode(sensor);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }
    sleep_for_ms(150);
    errorCode = adxl345bDeactivateMeasurementMode(sensor);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }
    for (size_t sampleIndex = 0; sampleIndex < numberOfSamples; sampleIndex++) {
        errorCode = adxl345bGetSingleMeasurement(sensor, &(sampleBuffer[sampleIndex]));
        if (errorCode != ADXL345B_NO_ERROR) {
            return errorCode;
        }
    }
    errorCode = adxl345bInternalClearFifoBuffer(sensor);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }

    // convert raw data to lsb values and calculate average
    int sumSamplesX = 0;
    int sumSamplesY = 0;
    int sumSamplesZ = 0;
    for (size_t sampleIndex = 0; sampleIndex < numberOfSamples; sampleIndex++) {
        sumSamplesX += adxl345bInternalConvertRawDataToLsbValue(
            sampleBuffer[sampleIndex].measurement_range, sampleBuffer[sampleIndex].full_res_mode,
            &(sampleBuffer[sampleIndex].rawData[0]));
        sumSamplesY += adxl345bInternalConvertRawDataToLsbValue(
            sampleBuffer[sampleIndex].measurement_range, sampleBuffer[sampleIndex].full_res_mode,
            &(sampleBuffer[sampleIndex].rawData[2]));
        sumSamplesZ += adxl345bInternalConvertRawDataToLsbValue(
            sampleBuffer[sampleIndex].measurement_range, sampleBuffer[sampleIndex].full_res_mode,
            &(sampleBuffer[sampleIndex].rawData[4]));
    }
    int avgSampleX = sumSamplesX / numberOfSamples;
    PRINT_DEBUG("X: %i LSB", avgSampleX);
    int avgSampleY = sumSamplesY / numberOfSamples;
    PRINT_DEBUG("Y: %i LSB", avgSampleY);
    int avgSampleZ = sumSamplesZ / numberOfSamples;
    PRINT_DEBUG("Z: %i LSB", avgSampleZ);

    // get offset based on average values X/Y should be 0G, Z should be -1G
    // 1G with 3,9mG/LSB -> 1000/3,9 LSB ~ 257 LSB
    int8_t offsetX = adxl345bInternalCalculateCalibrationOffset(avgSampleX);
    PRINT_DEBUG("offset X: %i LSB", offsetX);
    int8_t offsetY = adxl345bInternalCalculateCalibrationOffset(avgSampleY);
    PRINT_DEBUG("offset Y: %i LSB", offsetY);
    int8_t offsetZ = adxl345bInternalCalculateCalibrationOffset(avgSampleZ + 257);
    PRINT_DEBUG("offset Z: %i LSB", offsetZ);

    errorCode = adxl345bWriteConfigurationToSensor(sensor, ADXL345B_REGISTER_OFSX, offsetX);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }
    errorCode = adxl345bWriteConfigurationToSensor(sensor, ADXL345B_REGISTER_OFSY, offsetY);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }
    errorCode = adxl345bWriteConfigurationToSensor(sensor, ADXL345B_REGISTER_OFSZ, offsetZ);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }

    return adxl345bInternalWriteDefaultConfiguration(sensor);
}

// #############################################################################
// Data Acquisition
// #############################################################################

adxl345bErrorCode_t adxl345bCheckInterruptSet(adxl345bSensorConfiguration_t *sensor, uint8_t mask) {
    adxl345bErrorCode_t errorCode;
    uint8_t interruptRegister;
    errorCode = adxl345bReadConfigurationFromSensor(sensor, ADXL345B_REGISTER_INT_SOURCE,
                                                    &interruptRegister);
    PRINT_DEBUG("interrupt register: %02X", interruptRegister);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    } else if ((interruptRegister & mask) == mask) {
        return ADXL345B_NO_ERROR;
    } else {
        return ADXL345B_INTERRUP_NOT_SET;
    }
}

adxl345bErrorCode_t adxl345bConvertBytesToLSB(adxl345bRawData_t data, adxl345bLsbSample_t *sample) {
    sample->measurement_range = data.measurement_range;
    sample->full_res_mode = data.full_res_mode;
    sample->x = adxl345bInternalConvertRawDataToLsbValue(data.measurement_range, data.full_res_mode,
                                                         &(data.rawData[0]));
    sample->y = adxl345bInternalConvertRawDataToLsbValue(data.measurement_range, data.full_res_mode,
                                                         &(data.rawData[2]));
    sample->z = adxl345bInternalConvertRawDataToLsbValue(data.measurement_range, data.full_res_mode,
                                                         &(data.rawData[4]));
    return ADXL345B_NO_ERROR;
}

adxl345bErrorCode_t adxl345bConvertLSBToGValue(adxl345bLsbSample_t lsbValues,
                                               adxl345bGValueSample_t *gValues) {
    gValues->x = adxl345bInternalConvertLSBtoGValue(lsbValues.measurement_range,
                                                    lsbValues.full_res_mode, lsbValues.x);
    gValues->y = adxl345bInternalConvertLSBtoGValue(lsbValues.measurement_range,
                                                    lsbValues.full_res_mode, lsbValues.y);
    gValues->z = adxl345bInternalConvertLSBtoGValue(lsbValues.measurement_range,
                                                    lsbValues.full_res_mode, lsbValues.z);
    return ADXL345B_NO_ERROR;
}

adxl345bErrorCode_t adxl345bGetSingleMeasurement(adxl345bSensorConfiguration_t *sensor,
                                                 adxl345bRawData_t *sample) {
    sample->measurement_range = sensor->measurement_range;
    sample->full_res_mode = sensor->full_res_mode;
    return adxl345bInternalReadDataXYZ(sensor, sample->rawData);
}

// adxl345bErrorCode_t adxl345bGetMultipleMeasurements(adxl345bSensorConfiguration_t *sensor,
//                                                     adxl345bRawData_t *samples,
//                                                     uint32_t sizeOfSamples) {
//     for (size_t sampleIndex = 0; sampleIndex < sizeOfSamples; sampleIndex++) {
//         while (!adxl345bInternalCheckInterruptSet(sensor, ADXL345B_INTERRUPT_DATA_READY)) {
//             sleep_for_ms(1);
//         };
//
//         adxl345bErrorCode_t errorCode =
//             adxl345bInternalReadDataXYZ(sensor, samples[sampleIndex].rawData);
//         if (errorCode != ADXL345B_NO_ERROR) {
//             return errorCode;
//         }
//     }
//     return ADXL345B_NO_ERROR;
// }

// adxl345bErrorCode_t adxl345bGetMeasurementsForNMicroseconds(adxl345bSensorConfiguration_t
// *sensor,
//                                                             uint32_t timeToRead,
//                                                             adxl345bRawData_t *samples,
//                                                             size_t sizeOfSamples) {
//     uint32_t endTime = timeUs32() + timeToRead;
//     size_t sampleIndex = 0;
//     do {
//         while (!adxl345bInternalCheckInterruptSet(sensor, ADXL345B_INTERRUPT_DATA_READY)) {
//             sleep_for_ms(1);
//         };
//
//         adxl345bErrorCode_t errorCode =
//             adxl345bInternalReadDataXYZ(sensor, samples[sampleIndex].rawData);
//         if (errorCode != ADXL345B_NO_ERROR) {
//             return errorCode;
//         }
//
//         sampleIndex++;
//
//         if (sampleIndex >= sizeOfSamples) {
//             PRINT("buffer overflow, increase buffer size");
//             return ADXL345B_BUFFER_OVERFLOW_ERROR;
//         }
//     } while (timeUs32() <= endTime);
//
//     return ADXL345B_NO_ERROR;
// }
