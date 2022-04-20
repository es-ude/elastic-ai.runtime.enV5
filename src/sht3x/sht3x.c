//
// Created by David P. Federl
//

#include "sht3x.h"
#include "i2c/i2c.h"
#include <pico/stdlib.h>

/****************************************************/
/* region DATATYPES / VARIABLES */

static const uint16_t CRC8_POLYNOMIAL = 0x31;             /* P(x) = x^8 + x^5 + x^4 + 1 = 00110001= 0x31 */
static const float    DENOMINATOR     = (1 << 16) - 1.0f; /* 2^16 - 1 */

static sht3x_i2c_sensorConfiguration sensorConfiguration;

/* endregion */
/****************************************************/
/* region STATIC FUNCTION PROTOTYPES */

static sht3x_errorCode sendRequestToSensor(sht3x_command command);

static sht3x_errorCode receiveDataFromSensor(uint8_t *responseBuffer, uint8_t sizeOfResponseBuffer);

static sht3x_errorCode performChecksumCheck(const uint8_t *responseBuffer, uint8_t sizeOfResponseBuffer);

static uint8_t calculateChecksum(const uint8_t *dataBuffer);

static float calculateTemperature(uint16_t rawValue);

static float calculateHumidity(uint16_t rawValue);

/* endregion */
/****************************************************/
/* region HEADER FUNCTION IMPLEMENTATIONS */

sht3x_errorCode sht3x_init(i2c_inst_t *i2cHost) {
  /* needs max 1.5ms for idle state after power up */

  /* save i2c access for later usage */
  sensorConfiguration.i2c_host          = i2cHost;
  sensorConfiguration.i2c_slave_address = 0x44;

  I2C_Init(sensorConfiguration.i2c_host, (100 * 1000), 0, 1);

  /* Check if SHT3X is on Bus by requesting serialnumber without processing */
  uint8_t sizeOfCommandBuffer = 2;
  uint8_t commandBuffer[sizeOfCommandBuffer];
  commandBuffer[0] = (SHT3X_CMD_READ_STATUS >> 8);
  commandBuffer[1] = (SHT3X_CMD_READ_STATUS & 0xFF);

  I2C_ErrorCode errorCode =
      I2C_WriteCommand(commandBuffer, sizeOfCommandBuffer, sensorConfiguration.i2c_slave_address,
                       sensorConfiguration.i2c_host);
  if (errorCode != I2C_NO_ERROR) {
    return SHT3X_INIT_ERROR;
  }

  sleep_ms(2);

  return SHT3X_NO_ERROR;
}

sht3x_errorCode sht3x_readSerialNumber(uint32_t *serialNumber) {
  uint8_t sizeOfRequestBuffer = 6;
  uint8_t requestBuffer[sizeOfRequestBuffer];

  sht3x_errorCode errorCode = sendRequestToSensor(SHT3X_CMD_READ_SERIALNUMBER);
  if (errorCode != SHT3X_NO_ERROR) {
    return errorCode;
  }

  errorCode = receiveDataFromSensor(requestBuffer, sizeOfRequestBuffer);
  if (errorCode != SHT3X_NO_ERROR) {
    return errorCode;
  }

  errorCode = performChecksumCheck(requestBuffer, sizeOfRequestBuffer);
  if (errorCode != SHT3X_NO_ERROR) {
    return errorCode;
  }

  *serialNumber = ((uint32_t)(requestBuffer[0]) << 24) | ((uint32_t)(requestBuffer[1]) << 16) |
                  ((uint32_t)(requestBuffer[3]) << 8) | (uint32_t)(requestBuffer[4]);

  return SHT3X_NO_ERROR;
}

sht3x_errorCode sht3x_readStatusRegister(sht3x_statusRegister *statusRegister) {
  uint8_t sizeOfResponseBuffer = 3;
  uint8_t responseBuffer[sizeOfResponseBuffer];

  sht3x_errorCode sht3XErrorCode = sendRequestToSensor(SHT3X_CMD_READ_STATUS);
  if (sht3XErrorCode != SHT3X_NO_ERROR) {
    return sht3XErrorCode;
  }

  sht3XErrorCode = receiveDataFromSensor(responseBuffer, sizeOfResponseBuffer);
  if (sht3XErrorCode != SHT3X_NO_ERROR) {
    return sht3XErrorCode;
  }

  sht3XErrorCode = performChecksumCheck(responseBuffer, sizeOfResponseBuffer);
  if (sht3XErrorCode != SHT3X_NO_ERROR) {
    return sht3XErrorCode;
  }

  statusRegister->config = ((uint16_t)(responseBuffer[0]) << 8) | (uint16_t)(responseBuffer[1]);

  return SHT3X_NO_ERROR;
}

sht3x_errorCode sht3x_getTemperature(float *temperature) {
  uint8_t sizeOfResponseBuffer = 3;
  uint8_t responseBuffer[sizeOfResponseBuffer];

  sht3x_errorCode errorCode = sendRequestToSensor(SHT3X_CMD_MEASURE_CLOCKSTRETCH_LOW);
  if (errorCode != SHT3X_NO_ERROR) {
    return errorCode;
  }

  errorCode = receiveDataFromSensor(responseBuffer, sizeOfResponseBuffer);
  if (errorCode != SHT3X_NO_ERROR) {
    return errorCode;
  }

  errorCode = performChecksumCheck(responseBuffer, sizeOfResponseBuffer);
  if (errorCode != SHT3X_NO_ERROR) {
    return errorCode;
  }

  uint16_t rawTemperature = ((uint16_t)(responseBuffer[0]) << 8) | (uint16_t)(responseBuffer[1]);
  *temperature            = calculateTemperature(rawTemperature);

  return SHT3X_NO_ERROR;
}

sht3x_errorCode sht3x_getHumidity(float *humidity) {
  uint8_t sizeOfResponseBuffer = 6;
  uint8_t responseBuffer[sizeOfResponseBuffer];

  sht3x_errorCode errorCode = sendRequestToSensor(SHT3X_CMD_MEASURE_CLOCKSTRETCH_LOW);
  if (errorCode != SHT3X_NO_ERROR) {
    return errorCode;
  }

  errorCode = receiveDataFromSensor(responseBuffer, sizeOfResponseBuffer);
  if (errorCode != SHT3X_NO_ERROR) {
    return errorCode;
  }

  errorCode = performChecksumCheck(responseBuffer, sizeOfResponseBuffer);
  if (errorCode != SHT3X_NO_ERROR) {
    return errorCode;
  }

  uint16_t rawHumidity = ((uint16_t)(responseBuffer[3]) << 8) | (uint16_t)(responseBuffer[4]);
  *humidity            = calculateHumidity(rawHumidity);

  return SHT3X_NO_ERROR;
}

sht3x_errorCode sht3x_getTemperatureAndHumidity(float *temperature, float *humidity) {
  uint8_t sizeOfResponseBuffer = 6;
  uint8_t responseBuffer[sizeOfResponseBuffer];

  sht3x_errorCode errorCode = sendRequestToSensor(SHT3X_CMD_MEASURE_CLOCKSTRETCH_LOW);
  if (errorCode != SHT3X_NO_ERROR) {
    return errorCode;
  }

  errorCode = receiveDataFromSensor(responseBuffer, sizeOfResponseBuffer);
  if (errorCode != SHT3X_NO_ERROR) {
    return errorCode;
  }

  errorCode = performChecksumCheck(responseBuffer, sizeOfResponseBuffer);
  if (errorCode != SHT3X_NO_ERROR) {
    return errorCode;
  }

  uint16_t rawTemperature = ((uint16_t)(responseBuffer[0]) << 8) | (uint16_t)(responseBuffer[1]);
  *temperature            = calculateTemperature(rawTemperature);

  uint16_t rawHumidity    = ((uint16_t)(responseBuffer[3]) << 8) | (uint16_t)(responseBuffer[4]);
  *humidity               = calculateHumidity(rawHumidity);

  return SHT3X_NO_ERROR;
}

sht3x_errorCode sht3x_readMeasurementBuffer(float *temperature, float *humidity) {
  uint8_t sizeOfResponseBuffer = 6;
  uint8_t responseBuffer[sizeOfResponseBuffer];

  sht3x_errorCode errorCode = sendRequestToSensor(SHT3X_CMD_FETCH_DATA);
  if (errorCode != SHT3X_NO_ERROR) {
    return errorCode;
  }

  errorCode = receiveDataFromSensor(responseBuffer, sizeOfResponseBuffer);
  if (errorCode != SHT3X_NO_ERROR) {
    return errorCode;
  }

  errorCode = performChecksumCheck(responseBuffer, sizeOfResponseBuffer);
  if (errorCode != SHT3X_NO_ERROR) {
    return errorCode;
  }

  uint16_t rawTemperature = ((uint16_t)(responseBuffer[0]) << 8) | (uint16_t)(responseBuffer[1]);
  *temperature            = calculateTemperature(rawTemperature);

  uint16_t rawHumidity    = ((uint16_t)(responseBuffer[3]) << 8) | (uint16_t)(responseBuffer[4]);
  *humidity               = calculateHumidity(rawHumidity);

  return SHT3X_NO_ERROR;
}

sht3x_errorCode sht3x_enableHeater(void) {
  uint8_t sizeOfCommandBuffer = 2;
  uint8_t commandBuffer[sizeOfCommandBuffer];
  commandBuffer[0] = SHT3X_CMD_HEATER_ENABLE >> 8;
  commandBuffer[1] = SHT3X_CMD_HEATER_ENABLE & 0xFF;

  I2C_ErrorCode errorCode =
      I2C_WriteCommand(commandBuffer, sizeOfCommandBuffer, sensorConfiguration.i2c_slave_address,
                       sensorConfiguration.i2c_host);

  if (errorCode == I2C_NO_ERROR) {
    return SHT3X_NO_ERROR;
  }

  return SHT3X_SEND_COMMAND_ERROR;
}

sht3x_errorCode sht3x_disableHeater(void) {
  uint8_t sizeOfCommandBuffer = 2;
  uint8_t commandBuffer[sizeOfCommandBuffer];
  commandBuffer[0] = SHT3X_CMD_HEATER_DISABLE >> 8;
  commandBuffer[1] = SHT3X_CMD_HEATER_ENABLE & 0xFF;

  I2C_ErrorCode errorCode =
      I2C_WriteCommand(commandBuffer, sizeOfCommandBuffer, sensorConfiguration.i2c_slave_address,
                       sensorConfiguration.i2c_host);

  if (errorCode == I2C_NO_ERROR) {
    return SHT3X_NO_ERROR;
  }

  return SHT3X_SEND_COMMAND_ERROR;
}

sht3x_errorCode sht3x_softReset(void) {
  uint8_t sizeOfCommandBuffer = 2;
  uint8_t commandBuffer[sizeOfCommandBuffer];
  commandBuffer[0] = SHT3X_CMD_SOFT_RESET >> 8;
  commandBuffer[1] = SHT3X_CMD_HEATER_ENABLE & 0xFF;

  I2C_ErrorCode errorCode =
      I2C_WriteCommand(commandBuffer, sizeOfCommandBuffer, sensorConfiguration.i2c_slave_address,
                       sensorConfiguration.i2c_host);

  if (errorCode == I2C_NO_ERROR) {
    return SHT3X_NO_ERROR;
  }

  return SHT3X_SEND_COMMAND_ERROR;
}

/* endregion */
/****************************************************/
/* region STATIC FUNCTION IMPLEMENTATIONS */

static sht3x_errorCode sendRequestToSensor(sht3x_command command) {
  uint8_t sizeOfCommandBuffer = 2;
  uint8_t commandBuffer[sizeOfCommandBuffer];
  commandBuffer[0] = (command >> 8);
  commandBuffer[1] = (command & 0xFF);

  sht3x_errorCode errorCode =
      I2C_WriteCommand(commandBuffer, sizeOfCommandBuffer, sensorConfiguration.i2c_slave_address,
                       sensorConfiguration.i2c_host);

  if (errorCode == I2C_NO_ERROR) {
    return SHT3X_NO_ERROR;
  }

  return SHT3X_SEND_COMMAND_ERROR;
}

static sht3x_errorCode receiveDataFromSensor(uint8_t *responseBuffer, uint8_t sizeOfResponseBuffer) {
  sht3x_errorCode errorCode =
      I2C_ReadData(responseBuffer, sizeOfResponseBuffer, sensorConfiguration.i2c_slave_address,
                   sensorConfiguration.i2c_host);

  if (errorCode == I2C_NO_ERROR) {
    return SHT3X_NO_ERROR;
  }

  return SHT3X_RECEIVE_DATA_ERROR;
}

static sht3x_errorCode performChecksumCheck(const uint8_t *responseBuffer, uint8_t sizeOfResponseBuffer) {
  uint8_t numberOfTriplets = sizeOfResponseBuffer / 3;
  uint8_t group            = 0;

  while (group < numberOfTriplets) {
    uint8_t startIndex          = group * 3;
    uint8_t calculatedChecksum  = calculateChecksum(&(responseBuffer[startIndex]));
    uint8_t transmittedChecksum = responseBuffer[startIndex + 2];

    if (transmittedChecksum != calculatedChecksum) {
      return SHT3X_CHECKSUM_ERROR;
    }

    group++;
  }

  return SHT3X_NO_ERROR;
}

/* method calculates the checksum for 2 bytes with the crc8 algorithm
 * -> implementation based on the datasheet
 */
static uint8_t calculateChecksum(const uint8_t *dataBuffer) {
  uint8_t checksum = 0xFF;

  for (uint8_t byteCounter = 0; byteCounter < 2; byteCounter++) {
    checksum ^= (dataBuffer[byteCounter]);
    for (uint8_t bitMask = 8; bitMask > 0; --bitMask) {
      if (checksum & 0x80) {
        checksum = (checksum << 1) ^ CRC8_POLYNOMIAL;
      } else {
        checksum = (checksum << 1);
      }
    }
  }

  return checksum;
}

/* method transforms raw_value from sensor to actual value in degree Celsius
 * -> implementation based on the datasheet
 */
static float calculateTemperature(uint16_t rawValue) {
  return (175.0f * ((float)rawValue / DENOMINATOR)) - 45.0f;
}

/* method transforms raw_value from sensor to actual value in percent RH
 * -> implementation based on the datasheet
 */
static float calculateHumidity(uint16_t rawValue) { return 100.0f * ((float)rawValue / DENOMINATOR); }

/* endregion */
