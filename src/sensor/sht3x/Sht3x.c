#define SOURCE_FILE "src/sht3x/sht3x.c"

#include "Sht3x.h"
#include "Common.h"
#include "I2c.h"
#include "Sht3xInternal.h"
#include "Sht3xTypedefs.h"
#include "Sleep.h"

/* region VARIABLES */

static sht3xI2cSensorConfiguration_t sht3xSensorConfiguration; /*!< i2cConfig configuration for the
                                                                * sensor */

/* endregion */

/* region HEADER FUNCTION IMPLEMENTATIONS */

sht3xErrorCode_t sht3xInit(i2c_inst_t *i2cHost) {
    /* save i2cConfig access for later usage */
    sht3xSensorConfiguration.i2c_host = i2cHost;
    sht3xSensorConfiguration.i2c_slave_address = 0x44;

    i2cInit(result);

    /* sleep to make sure the sensor is fully initialized */
    sleep_for_ms(2);

    /* check if SHT3X is on the Bus by requesting serial number without processing
     */
    uint8_t sizeOfCommandBuffer = 2;
    uint8_t commandBuffer[sizeOfCommandBuffer];
    commandBuffer[0] = (SHT3X_CMD_READ_STATUS >> 8);
    commandBuffer[1] = (SHT3X_CMD_READ_STATUS & 0xFF);

    /* if i2cConfig returns error -> sensor not available on bus */
    i2cErrorCode_t errorCode = i2cWriteCommand(sht3xSensorConfiguration.i2c_host,
                                               sht3xSensorConfiguration.i2c_slave_address,
                                               commandBuffer, sizeOfCommandBuffer);
    if (errorCode != I2C_NO_ERROR) {
        return SHT3X_INIT_ERROR;
    }

    return SHT3X_NO_ERROR;
}

sht3xErrorCode_t sht3xReadSerialNumber(uint32_t *serialNumber) {
    uint8_t sizeOfRequestBuffer = 6;
    uint8_t requestBuffer[sizeOfRequestBuffer];

    sht3xErrorCode_t errorCode = sht3xInternalSendRequestToSensor(SHT3X_CMD_READ_SERIALNUMBER);
    if (errorCode != SHT3X_NO_ERROR) {
        return errorCode;
    }

    errorCode = sht3xInternalReceiveDataFromSensor(requestBuffer, sizeOfRequestBuffer);
    if (errorCode != SHT3X_NO_ERROR) {
        return errorCode;
    }

    errorCode = sht3xInternalPerformChecksumCheck(requestBuffer, sizeOfRequestBuffer);
    if (errorCode != SHT3X_NO_ERROR) {
        return errorCode;
    }

    *serialNumber = ((uint32_t)(requestBuffer[0]) << 24) | ((uint32_t)(requestBuffer[1]) << 16) |
                    ((uint32_t)(requestBuffer[3]) << 8) | (uint32_t)(requestBuffer[4]);

    return SHT3X_NO_ERROR;
}

sht3xErrorCode_t sht3xReadStatusRegister(sht3xStatusRegister_t *statusRegister) {
    uint8_t sizeOfResponseBuffer = 3;
    uint8_t responseBuffer[sizeOfResponseBuffer];

    sht3xErrorCode_t errorCode = sht3xInternalSendRequestToSensor(SHT3X_CMD_READ_STATUS);
    if (errorCode != SHT3X_NO_ERROR) {
        return errorCode;
    }

    errorCode = sht3xInternalReceiveDataFromSensor(responseBuffer, sizeOfResponseBuffer);
    if (errorCode != SHT3X_NO_ERROR) {
        return errorCode;
    }

    errorCode = sht3xInternalPerformChecksumCheck(responseBuffer, sizeOfResponseBuffer);
    if (errorCode != SHT3X_NO_ERROR) {
        return errorCode;
    }

    statusRegister->config = ((uint16_t)(responseBuffer[0]) << 8) | (uint16_t)(responseBuffer[1]);

    return SHT3X_NO_ERROR;
}

sht3xErrorCode_t sht3xGetTemperature(float *temperature) {
    uint8_t sizeOfResponseBuffer = 3;
    uint8_t responseBuffer[sizeOfResponseBuffer];

    sht3xErrorCode_t errorCode =
        sht3xInternalSendRequestToSensor(SHT3X_CMD_MEASURE_CLOCKSTRETCH_LOW);
    if (errorCode != SHT3X_NO_ERROR) {
        return errorCode;
    }

    errorCode = sht3xInternalReceiveDataFromSensor(responseBuffer, sizeOfResponseBuffer);
    if (errorCode != SHT3X_NO_ERROR) {
        return errorCode;
    }

    errorCode = sht3xInternalPerformChecksumCheck(responseBuffer, sizeOfResponseBuffer);
    if (errorCode != SHT3X_NO_ERROR) {
        return errorCode;
    }

    uint16_t rawTemperature = ((uint16_t)(responseBuffer[0]) << 8) | (uint16_t)(responseBuffer[1]);
    *temperature = sht3xInternalCalculateTemperature(rawTemperature);

    return SHT3X_NO_ERROR;
}

sht3xErrorCode_t sht3xGetHumidity(float *humidity) {
    uint8_t sizeOfResponseBuffer = 6;
    uint8_t responseBuffer[sizeOfResponseBuffer];

    sht3xErrorCode_t errorCode =
        sht3xInternalSendRequestToSensor(SHT3X_CMD_MEASURE_CLOCKSTRETCH_LOW);
    if (errorCode != SHT3X_NO_ERROR) {
        return errorCode;
    }

    errorCode = sht3xInternalReceiveDataFromSensor(responseBuffer, sizeOfResponseBuffer);
    if (errorCode != SHT3X_NO_ERROR) {
        return errorCode;
    }

    errorCode = sht3xInternalPerformChecksumCheck(responseBuffer, sizeOfResponseBuffer);
    if (errorCode != SHT3X_NO_ERROR) {
        return errorCode;
    }

    uint16_t rawHumidity = ((uint16_t)(responseBuffer[3]) << 8) | (uint16_t)(responseBuffer[4]);
    *humidity = sht3xInternalCalculateHumidity(rawHumidity);

    return SHT3X_NO_ERROR;
}

sht3xErrorCode_t sht3xGetTemperatureAndHumidity(float *temperature, float *humidity) {
    uint8_t sizeOfResponseBuffer = 6;
    uint8_t responseBuffer[sizeOfResponseBuffer];

    sht3xErrorCode_t errorCode =
        sht3xInternalSendRequestToSensor(SHT3X_CMD_MEASURE_CLOCKSTRETCH_LOW);
    if (errorCode != SHT3X_NO_ERROR) {
        return errorCode;
    }

    errorCode = sht3xInternalReceiveDataFromSensor(responseBuffer, sizeOfResponseBuffer);
    if (errorCode != SHT3X_NO_ERROR) {
        return errorCode;
    }

    errorCode = sht3xInternalPerformChecksumCheck(responseBuffer, sizeOfResponseBuffer);
    if (errorCode != SHT3X_NO_ERROR) {
        return errorCode;
    }

    uint16_t rawTemperature = ((uint16_t)(responseBuffer[0]) << 8) | (uint16_t)(responseBuffer[1]);
    *temperature = sht3xInternalCalculateTemperature(rawTemperature);

    uint16_t rawHumidity = ((uint16_t)(responseBuffer[3]) << 8) | (uint16_t)(responseBuffer[4]);
    *humidity = sht3xInternalCalculateHumidity(rawHumidity);

    return SHT3X_NO_ERROR;
}

sht3xErrorCode_t sht3xReadMeasurementBuffer(float *temperature, float *humidity) {
    uint8_t sizeOfResponseBuffer = 6;
    uint8_t responseBuffer[sizeOfResponseBuffer];

    sht3xErrorCode_t errorCode = sht3xInternalSendRequestToSensor(SHT3X_CMD_FETCH_DATA);
    if (errorCode != SHT3X_NO_ERROR) {
        return errorCode;
    }

    errorCode = sht3xInternalReceiveDataFromSensor(responseBuffer, sizeOfResponseBuffer);
    if (errorCode != SHT3X_NO_ERROR) {
        return errorCode;
    }

    errorCode = sht3xInternalPerformChecksumCheck(responseBuffer, sizeOfResponseBuffer);
    if (errorCode != SHT3X_NO_ERROR) {
        return errorCode;
    }

    uint16_t rawTemperature = ((uint16_t)(responseBuffer[0]) << 8) | (uint16_t)(responseBuffer[1]);
    *temperature = sht3xInternalCalculateTemperature(rawTemperature);

    uint16_t rawHumidity = ((uint16_t)(responseBuffer[3]) << 8) | (uint16_t)(responseBuffer[4]);
    *humidity = sht3xInternalCalculateHumidity(rawHumidity);

    return SHT3X_NO_ERROR;
}

sht3xErrorCode_t sht3xEnableHeater(void) {
    return sht3xInternalSendRequestToSensor(SHT3X_CMD_HEATER_ENABLE);
}

sht3xErrorCode_t sht3xDisableHeater(void) {
    return sht3xInternalSendRequestToSensor(SHT3X_CMD_HEATER_DISABLE);
}

sht3xErrorCode_t sht3xSoftReset(void) {
    return sht3xInternalSendRequestToSensor(SHT3X_CMD_SOFT_RESET);
}

/* endregion */

/* region STATIC FUNCTION IMPLEMENTATIONS */

static sht3xErrorCode_t sht3xInternalSendRequestToSensor(sht3xCommand_t command) {
    uint8_t sizeOfCommandBuffer = 2;
    uint8_t commandBuffer[sizeOfCommandBuffer];
    commandBuffer[0] = (command >> 8);
    commandBuffer[1] = (command & 0xFF);

    PRINT_DEBUG("requesting data from sensor");
    sht3xErrorCode_t errorCode = i2cWriteCommand(sht3xSensorConfiguration.i2c_host,
                                                 sht3xSensorConfiguration.i2c_slave_address,
                                                 commandBuffer, sizeOfCommandBuffer);

    if (errorCode == I2C_NO_ERROR) {
        return SHT3X_NO_ERROR;
    }

    PRINT_DEBUG("sending request failed, error was %02X", errorCode);
    return SHT3X_SEND_COMMAND_ERROR;
}

static sht3xErrorCode_t sht3xInternalReceiveDataFromSensor(uint8_t *responseBuffer,
                                                           uint8_t sizeOfResponseBuffer) {
    PRINT_DEBUG("receiving data from sensor");
    sht3xErrorCode_t errorCode =
        i2cReadData(sht3xSensorConfiguration.i2c_host, sht3xSensorConfiguration.i2c_slave_address,
                    responseBuffer, sizeOfResponseBuffer);

    if (errorCode == I2C_NO_ERROR) {
        return SHT3X_NO_ERROR;
    }

    PRINT_DEBUG("receiving data failed, error was %02X", errorCode);
    return SHT3X_RECEIVE_DATA_ERROR;
}

static sht3xErrorCode_t sht3xInternalPerformChecksumCheck(const uint8_t *responseBuffer,
                                                          uint8_t sizeOfResponseBuffer) {
    PRINT_DEBUG("performing checksum check");
    uint8_t numberOfTriplets = sizeOfResponseBuffer / 3;
    uint8_t group = 0;

    while (group < numberOfTriplets) {
        uint8_t startIndex = group * 3;
        uint8_t calculatedChecksum = sht3xInternalCalculateChecksum(&(responseBuffer[startIndex]));
        uint8_t transmittedChecksum = responseBuffer[startIndex + 2];

        if (transmittedChecksum != calculatedChecksum) {
            PRINT_DEBUG("checksum failed, input: %i, calculated: %i", transmittedChecksum,
                        calculatedChecksum);
            return SHT3X_CHECKSUM_ERROR;
        }

        group++;
    }

    PRINT_DEBUG("checksum passed");
    return SHT3X_NO_ERROR;
}

static uint8_t sht3xInternalCalculateChecksum(const uint8_t *dataBuffer) {
    uint8_t checksum = 0xFF;

    for (uint8_t byteCounter = 0; byteCounter < 2; byteCounter++) {
        checksum ^= (dataBuffer[byteCounter]);
        for (uint8_t bitMask = 8; bitMask > 0; --bitMask) {
            if (checksum & 0x80) {
                checksum = (checksum << 1) ^ sht3xCrc8Polynomial;
            } else {
                checksum = (checksum << 1);
            }
        }
    }

    return checksum;
}

static float sht3xInternalCalculateTemperature(uint16_t rawValue) {
    float temperature = (175.0f * ((float)rawValue / sht3xDenominator)) - 45.0f;
    PRINT_DEBUG("calculate the temperature, temperature=%f", temperature);
    return temperature;
}

static float sht3xInternalCalculateHumidity(uint16_t rawValue) {
    float humidity = 100.0f * ((float)rawValue / sht3xDenominator);
    PRINT_DEBUG("calculating humidity, humidity=%f", humidity);
    return humidity;
}

/* endregion */
