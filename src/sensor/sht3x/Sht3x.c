#define SOURCE_FILE "src/sht3x/sht3x.c"

#include "CException.h"

#include "Common.h"
#include "I2c.h"
#include "Sht3xInternal.h"
#include "include/Sht3x.h"
#include "include/Sht3xTypedefs.h"

const uint16_t sht3xCrc8Polynomial = 0x31;       //!< P(x) = x^8 + x^5 + x^4 + 1 = 0b00110001 = 0x31
const float sht3xDenominator = (1 << 16) - 1.0f; //!<  2^16 - 1 = 0b1111111111111111 = 0xFFFF

/* region HEADER FUNCTION IMPLEMENTATIONS */

void sht3xInit(sht3xSensorConfiguration_t sensor) {
    /* check if SHT3X is on the Bus by requesting serial number without processing */
    uint8_t sizeOfCommandBuffer = 2;
    uint8_t commandBuffer[sizeOfCommandBuffer];
    commandBuffer[0] = (SHT3X_CMD_READ_STATUS >> 8);
    commandBuffer[1] = (SHT3X_CMD_READ_STATUS & 0xFF);
    i2cErrorCode_t errorCode = i2cWriteCommand(sensor.i2c_host, sensor.i2c_slave_address,
                                               commandBuffer, sizeOfCommandBuffer);

    if (I2C_INIT_ERROR == errorCode) {
        Throw(SHT3X_HARDWARE_ERROR);
    } else if (I2C_NO_ERROR != errorCode) {
        Throw(SHT3X_SEND_COMMAND_ERROR);
    }
}

uint32_t sht3xReadSerialNumber(sht3xSensorConfiguration_t sensor) {
    uint8_t sizeOfRequestBuffer = 6;
    uint8_t requestBuffer[sizeOfRequestBuffer];

    sht3xInternalSendRequestToSensor(sensor, SHT3X_CMD_READ_SERIALNUMBER);
    sht3xInternalReceiveDataFromSensor(sensor, requestBuffer, sizeOfRequestBuffer);
    sht3xInternalPerformChecksumCheck(requestBuffer, sizeOfRequestBuffer);

    return ((uint32_t)(requestBuffer[0]) << 24) | ((uint32_t)(requestBuffer[1]) << 16) |
           ((uint32_t)(requestBuffer[3]) << 8) | (uint32_t)(requestBuffer[4]);
}

sht3xStatusRegister_t sht3xReadStatusRegister(sht3xSensorConfiguration_t sensor) {
    uint8_t sizeOfResponseBuffer = 3;
    uint8_t responseBuffer[sizeOfResponseBuffer];

    sht3xInternalSendRequestToSensor(sensor, SHT3X_CMD_READ_STATUS);
    sht3xInternalReceiveDataFromSensor(sensor, responseBuffer, sizeOfResponseBuffer);
    sht3xInternalPerformChecksumCheck(responseBuffer, sizeOfResponseBuffer);

    return (sht3xStatusRegister_t){.config = ((uint16_t)(responseBuffer[0]) << 8) |
                                             (uint16_t)(responseBuffer[1])};
}

float sht3xGetTemperature(sht3xSensorConfiguration_t sensor) {
    uint8_t sizeOfResponseBuffer = 3;
    uint8_t responseBuffer[sizeOfResponseBuffer];

    sht3xInternalSendRequestToSensor(sensor, SHT3X_CMD_MEASURE_CLOCKSTRETCH_LOW);
    sht3xInternalReceiveDataFromSensor(sensor, responseBuffer, sizeOfResponseBuffer);
    sht3xInternalPerformChecksumCheck(responseBuffer, sizeOfResponseBuffer);

    uint16_t rawTemperature = ((uint16_t)(responseBuffer[0]) << 8) | (uint16_t)(responseBuffer[1]);
    return sht3xInternalCalculateTemperature(rawTemperature);
}

float sht3xGetHumidity(sht3xSensorConfiguration_t sensor) {
    uint8_t sizeOfResponseBuffer = 6;
    uint8_t responseBuffer[sizeOfResponseBuffer];

    sht3xInternalSendRequestToSensor(sensor, SHT3X_CMD_MEASURE_CLOCKSTRETCH_LOW);
    sht3xInternalReceiveDataFromSensor(sensor, responseBuffer, sizeOfResponseBuffer);
    sht3xInternalPerformChecksumCheck(responseBuffer, sizeOfResponseBuffer);

    uint16_t rawHumidity = ((uint16_t)(responseBuffer[3]) << 8) | (uint16_t)(responseBuffer[4]);
    return sht3xInternalCalculateHumidity(rawHumidity);
}

void sht3xGetTemperatureAndHumidity(sht3xSensorConfiguration_t sensor, float *temperature,
                                    float *humidity) {
    uint8_t sizeOfResponseBuffer = 6;
    uint8_t responseBuffer[sizeOfResponseBuffer];

    sht3xInternalSendRequestToSensor(sensor, SHT3X_CMD_MEASURE_CLOCKSTRETCH_LOW);
    sht3xInternalReceiveDataFromSensor(sensor, responseBuffer, sizeOfResponseBuffer);
    sht3xInternalPerformChecksumCheck(responseBuffer, sizeOfResponseBuffer);

    uint16_t rawTemperature = ((uint16_t)(responseBuffer[0]) << 8) | (uint16_t)(responseBuffer[1]);
    *temperature = sht3xInternalCalculateTemperature(rawTemperature);

    uint16_t rawHumidity = ((uint16_t)(responseBuffer[3]) << 8) | (uint16_t)(responseBuffer[4]);
    *humidity = sht3xInternalCalculateHumidity(rawHumidity);
}

void sht3xReadMeasurementBuffer(sht3xSensorConfiguration_t sensor, float *temperature,
                                float *humidity) {
    uint8_t sizeOfResponseBuffer = 6;
    uint8_t responseBuffer[sizeOfResponseBuffer];

    sht3xInternalSendRequestToSensor(sensor, SHT3X_CMD_FETCH_DATA);
    sht3xInternalReceiveDataFromSensor(sensor, responseBuffer, sizeOfResponseBuffer);
    sht3xInternalPerformChecksumCheck(responseBuffer, sizeOfResponseBuffer);

    uint16_t rawTemperature = ((uint16_t)(responseBuffer[0]) << 8) | (uint16_t)(responseBuffer[1]);
    *temperature = sht3xInternalCalculateTemperature(rawTemperature);

    uint16_t rawHumidity = ((uint16_t)(responseBuffer[3]) << 8) | (uint16_t)(responseBuffer[4]);
    *humidity = sht3xInternalCalculateHumidity(rawHumidity);
}

void sht3xEnableHeater(sht3xSensorConfiguration_t sensor) {
    sht3xInternalSendRequestToSensor(sensor, SHT3X_CMD_HEATER_ENABLE);
}

void sht3xDisableHeater(sht3xSensorConfiguration_t sensor) {
    sht3xInternalSendRequestToSensor(sensor, SHT3X_CMD_HEATER_DISABLE);
}

void sht3xSoftReset(sht3xSensorConfiguration_t sensor) {
    sht3xInternalSendRequestToSensor(sensor, SHT3X_CMD_SOFT_RESET);
}

/* endregion */

/* region STATIC FUNCTION IMPLEMENTATIONS */

static void sht3xInternalSendRequestToSensor(sht3xSensorConfiguration_t sensor,
                                             sht3xCommand_t command) {
    uint8_t sizeOfCommandBuffer = 2;
    uint8_t commandBuffer[sizeOfCommandBuffer];
    commandBuffer[0] = (command >> 8);
    commandBuffer[1] = (command & 0xFF);

    PRINT_DEBUG("requesting data from sensor");
    sht3xErrorCode_t errorCode = i2cWriteCommand(sensor.i2c_host, sensor.i2c_slave_address,
                                                 commandBuffer, sizeOfCommandBuffer);

    if (errorCode == I2C_INIT_ERROR) {
        PRINT_DEBUG("sending request failed, error was %02X", errorCode);
        Throw(SHT3X_HARDWARE_ERROR);
    } else if (errorCode != I2C_NO_ERROR) {
        PRINT_DEBUG("sending request failed, error was %02X", errorCode);
        Throw(SHT3X_SEND_COMMAND_ERROR);
    }
}

static void sht3xInternalReceiveDataFromSensor(sht3xSensorConfiguration_t sensor,
                                               uint8_t *responseBuffer,
                                               uint8_t sizeOfResponseBuffer) {
    PRINT_DEBUG("receiving data from sensor");
    sht3xErrorCode_t errorCode = i2cReadData(sensor.i2c_host, sensor.i2c_slave_address,
                                             responseBuffer, sizeOfResponseBuffer);

    if (errorCode == I2C_INIT_ERROR) {
        PRINT_DEBUG("receiving data failed, error was %02X", errorCode);
        Throw(SHT3X_HARDWARE_ERROR);
    } else if (errorCode != I2C_NO_ERROR) {
        PRINT_DEBUG("receiving data failed, error was %02X", errorCode);
        Throw(SHT3X_RECEIVE_DATA_ERROR);
    }
}

static void sht3xInternalPerformChecksumCheck(const uint8_t *responseBuffer,
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
            Throw(SHT3X_CHECKSUM_ERROR);
        }

        group++;
    }

    PRINT_DEBUG("checksum matches!");
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
