#define SOURCE_FILE "ADXL345B-LIB-INTERNAL"

#include "Adxl345bInternal.h"
#include "eai/Common.h"
#include "eai/hal/I2c.h"
#include "eai/hal/Sleep.h"
#include "eai/sensor/Adxl345b.h"
#include "eai/sensor/Adxl345bTypedefs.h"

// #############################################################################
// Configuration
// #############################################################################

adxl345bErrorCode_t
adxl345bInternalWriteDefaultConfiguration(adxl345bSensorConfiguration_t *sensor) {
    adxl345bErrorCode_t errorCode;
    errorCode = adxl345bWriteConfigurationToSensor(sensor, ADXL345B_REGISTER_BW_RATE,
                                                   ADXL345B_BW_RATE_LPM_12_point_5);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }
    errorCode = adxl345bWriteConfigurationToSensor(sensor, ADXL345B_REGISTER_POWER_CTL, 0b00000000);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }
    errorCode =
        adxl345bWriteConfigurationToSensor(sensor, ADXL345B_REGISTER_INT_ENABLE, 0b00000000);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }
    errorCode = adxl345bWriteConfigurationToSensor(sensor, ADXL345B_REGISTER_DATA_FORMAT,
                                                   ADXL345B_2G_RANGE);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }
    return adxl345bWriteConfigurationToSensor(sensor, ADXL345B_FIFO_CTL, ADXL345B_FIFOMODE_FIFO);
}

adxl345bErrorCode_t adxl345bInternalEnableSelfTestForce(adxl345bSensorConfiguration_t *sensor) {
    adxl345bErrorCode_t errorCode;

    // enable self-test
    uint8_t data_format_register;
    errorCode = adxl345bReadConfigurationFromSensor(sensor, ADXL345B_REGISTER_DATA_FORMAT,
                                                    &data_format_register);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }

    errorCode =
        adxl345bWriteConfigurationToSensor(sensor, ADXL345B_REGISTER_DATA_FORMAT,
                                           (data_format_register | ADXL345B_DATA_FORMAT_SELF_TEST));
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }

    // idle after settings change
    sleep_for_ms(2);

    return ADXL345B_NO_ERROR;
}

adxl345bErrorCode_t adxl345bInternalDisableSelfTestForce(adxl345bSensorConfiguration_t *sensor) {
    adxl345bErrorCode_t errorCode;

    uint8_t data_format_register;
    errorCode = adxl345bReadConfigurationFromSensor(sensor, ADXL345B_REGISTER_DATA_FORMAT,
                                                    &data_format_register);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }

    errorCode = adxl345bWriteConfigurationToSensor(
        sensor, ADXL345B_REGISTER_DATA_FORMAT,
        (data_format_register & (~ADXL345B_DATA_FORMAT_SELF_TEST)));
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }

    // idle after settings change
    sleep_for_ms(2);

    return ADXL345B_NO_ERROR;
}

// #############################################################################
// Calibration
// #############################################################################

int8_t adxl345bInternalCalculateCalibrationOffset(int16_t measured) {
    PRINT_DEBUG("INPUT: %i", measured);
    int16_t offset = (-1) * (measured / 4);
    PRINT_DEBUG("TRANSFORMED: %i", offset);

    if (offset > INT8_MAX) {
        PRINT_DEBUG("RET MAX");
        return INT8_MAX;
    } else if (offset < INT8_MIN) {
        PRINT_DEBUG("RET MIN");
        return INT8_MIN;
    } else {
        PRINT_DEBUG("RET %i", (int8_t)offset);
        return (int8_t)offset;
    }
}

// #############################################################################
// Data Conversion
// #############################################################################

int16_t adxl345bInternalConvertRawDataToLsbValue(adxl345bRange_t valueRange, bool isFullResolution,
                                                 uint8_t rawValue[2]) {
    uint16_t mask;
    uint16_t msb;
    if (isFullResolution) {
        // 10-bit mode + range bits
        mask = (1 << (10 + valueRange)) - 1;
        msb = 1 << (10 + valueRange - 1);
    } else {
        // 10-bit mode
        mask = (1 << 10) - 1;
        msb = (1 << 9);
    }
    PRINT_DEBUG("range ID: 0x%02X", valueRange);
    PRINT_DEBUG("bit mask: 0x%04X -- msb: 0x%04X", mask, msb);

    uint16_t bytes = (((uint16_t)rawValue[1] << 8) | (uint16_t)rawValue[0]) & mask;
    PRINT_DEBUG("raw: 0x%04X", bytes);

    int16_t lsbValue;
    if ((bytes & msb) == 0) {
        lsbValue = (int16_t)bytes;
    } else {
        lsbValue = (int16_t)(bytes | ~mask);
    }

    PRINT_DEBUG("LSB: %i", lsbValue);
    return lsbValue;
}

float adxl345bInternalConvertLSBtoGValue(adxl345bRange_t valueRange, bool isFullResolution,
                                         int rawValue) {
    float scaleFactor;
    if (isFullResolution) {
        scaleFactor = 0.0039f; // 3.9 mg/LSB
    } else {
        switch (valueRange) {
        case ADXL345B_16G_RANGE:
            scaleFactor = 0.0312f; // 31.2 mg/LSB
            break;
        case ADXL345B_8G_RANGE:
            scaleFactor = 0.0156f; // 15.6 mg/LSB
            break;
        case ADXL345B_4G_RANGE:
            scaleFactor = 0.0078f; // 7.8 mg/LSB
            break;
        case ADXL345B_2G_RANGE:
        default:
            scaleFactor = 0.0039f; // 3.9 mg/LSB
            break;
        }
    }

    float gValue = (float)rawValue * scaleFactor;
    PRINT_DEBUG("LSB: %i, G value: %f", rawValue, gValue);
    return gValue;
}

// #############################################################################
// Data Handling
// #############################################################################

adxl345bErrorCode_t adxl345bInternalReadDataXYZ(adxl345bSensorConfiguration_t *sensor,
                                                uint8_t rawData[6]) {
    adxl345bErrorCode_t errorCode;

    i2cErrorCode_t i2cErrorCode;
    uint8_t commandBuffer[1];
    commandBuffer[0] = ADXL345B_REGISTER_DATA_X;

    i2cErrorCode = i2cWriteCommand(sensor->i2c_host, sensor->i2c_slave_address, commandBuffer, 1);
    if (i2cErrorCode != I2C_NO_ERROR) {
        return ADXL345B_SEND_COMMAND_ERROR;
    }

    i2cErrorCode = i2cReadData(sensor->i2c_host, sensor->i2c_slave_address, rawData, 6);
    if (i2cErrorCode != I2C_NO_ERROR) {
        return ADXL345B_RECEIVE_DATA_ERROR;
    }
    return ADXL345B_NO_ERROR;
}

adxl345bErrorCode_t adxl345bInternalClearFifoBuffer(adxl345bSensorConfiguration_t *sensor) {
    adxl345bErrorCode_t errorCode;

    uint8_t samplesInFifo;
    errorCode = adxl345bReadConfigurationFromSensor(sensor, ADXL345B_FIFO_STATUS,
                                                    (uint8_t *)&samplesInFifo);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }
    samplesInFifo &= 0b00111111;
    PRINT_DEBUG("Samples in FIFO: %u", samplesInFifo);

    for (size_t index = 0; index <= samplesInFifo; index++) {
        errorCode = adxl345bInternalReadDataXYZ(sensor, NULL);
        if (errorCode != ADXL345B_NO_ERROR) {
            return errorCode;
        }
    }
    return ADXL345B_NO_ERROR;
}
