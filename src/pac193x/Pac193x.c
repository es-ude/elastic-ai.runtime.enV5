#define SOURCE_FILE "src/pac193x/pac193x.c"

#include "Pac193x.h"
#include "Common.h"
#include "Gpio.h"
#include "I2c.h"
#include "Pac193xInternal.h"
#include <pico/time.h>
#include <stdbool.h>

/* region VARIABLES */

static pac193xSensorConfiguration_t pac193xSensorConfiguration;

/* endregion */

/* region HEADER FUNCTION IMPLEMENTATIONS */

pac193xErrorCode_t pac193xPowerUpSensor(void) {
    gpioErrorCode_t gpioErrorCode = gpioSetPower(29, GPIO_POWER_UP);
    if (gpioErrorCode != GPIO_NO_ERROR) {
        return PAC193X_INIT_ERROR;
    }

    return PAC193X_NO_ERROR;
}

pac193xErrorCode_t pac193xPowerDownSensor(void) {
    gpioErrorCode_t gpioErrorCode = gpioSetPower(29, GPIO_POWER_DOWN);
    if (gpioErrorCode != GPIO_NO_ERROR) {
        return PAC193X_INIT_ERROR;
    }

    return PAC193X_NO_ERROR;
}

pac193xErrorCode_t pac193xInit(i2c_inst_t *i2cHost, pac193xI2cAddress slaveAddress,
                               float resistanceValues[4], pac193xUsedChannels_t usedChannels) {
    /* save i2c access for later usage */
    pac193xSensorConfiguration.i2c_host = i2cHost;
    pac193xSensorConfiguration.i2c_slave_address = slaveAddress;

    /* power up sensor */
    pac193xErrorCode_t errorCode = pac193xPowerUpSensor();
    if (errorCode != PAC193X_NO_ERROR) {
        return PAC193X_INIT_ERROR;
    }

    /* sleep to make sure the sensor is fully initialized  */
    sleep_ms(10);

    i2cInit(pac193xSensorConfiguration.i2c_host, (100 * 1000), 6, 7);

    /* check if pac193x is available on bus */
    uint8_t sizeOfCommandBuffer = 1;
    uint8_t commandBuffer[sizeOfCommandBuffer];
    commandBuffer[0] = PAC193X_CMD_READ_MANUFACTURER_ID;

    /* if i2c returns error -> sensor not available on bus */
    i2cErrorCode_t i2CErrorCode = i2cWriteCommand(commandBuffer, sizeOfCommandBuffer,
                                                  pac193xSensorConfiguration.i2c_slave_address,
                                                  pac193xSensorConfiguration.i2c_host);
    if (i2CErrorCode != I2C_NO_ERROR) {
        return PAC193X_INIT_ERROR;
    }

    errorCode = pac193xSetResistanceAtSense(resistanceValues);
    if (errorCode != PAC193X_NO_ERROR) {
        return PAC193X_INIT_ERROR;
    }

    errorCode = pac193xSetChannelsInUse(usedChannels);
    if (errorCode != PAC193X_NO_ERROR) {
        return PAC193X_INIT_ERROR;
    }

    /* sample rate of 1024Hz, enable continuous measurement mode, enable
     * overflow alert */
    errorCode = pac193xInternalSendConfigurationToSensor(PAC193X_CMD_CTRL, 0b00001010);
    if (errorCode != PAC193X_NO_ERROR) {
        return PAC193X_INIT_ERROR;
    }

    /* sets measurement to unipolar mode */
    errorCode = pac193xInternalSendConfigurationToSensor(PAC193X_CMD_NEG_PWR, 0b00000000);
    if (errorCode != PAC193X_NO_ERROR) {
        return PAC193X_INIT_ERROR;
    }

    /* enable limited refresh function and set POR to 0 */
    errorCode = pac193xInternalSendConfigurationToSensor(PAC193X_CMD_SLOW, 0b00010100);
    if (errorCode != PAC193X_NO_ERROR) {
        return PAC193X_INIT_ERROR;
    }

    /* refresh sensor to use updated settings */
    errorCode = pac193xInternalRefresh();
    if (errorCode != PAC193X_NO_ERROR) {
        return PAC193X_INIT_ERROR;
    }

    return PAC193X_NO_ERROR;
}

pac193xErrorCode_t pac193xSetResistanceAtSense(const float resistanceValues[4]) {
    pac193xSensorConfiguration.rSense[0] = resistanceValues[0];
    pac193xSensorConfiguration.rSense[1] = resistanceValues[1];
    pac193xSensorConfiguration.rSense[2] = resistanceValues[2];
    pac193xSensorConfiguration.rSense[3] = resistanceValues[3];

    return PAC193X_NO_ERROR;
}

pac193xErrorCode_t pac193xSetChannelsInUse(pac193xUsedChannels_t usedChannels) {
    /* check if used channels are valid */
    if (usedChannels.uint_channelsInUse > 0b00010000) {
        return PAC193X_INIT_ERROR;
    }

    /* update configuration */
    pac193xSensorConfiguration.usedChannels = usedChannels;

    /* send configuration to sensor */
    uint8_t channelsInUse = (usedChannels.uint_channelsInUse << 4) ^ 0xF0;
    pac193xErrorCode_t errorCode =
        pac193xInternalSendConfigurationToSensor(PAC193X_CMD_CHANNEL_DIS, channelsInUse);
    if (errorCode != PAC193X_NO_ERROR) {
        return PAC193X_INIT_ERROR;
    }

    return PAC193X_NO_ERROR;
}

pac193xErrorCode_t pac193xGetSensorInfo(pac193xSensorId_t *info) {
    uint8_t sizeOfResponseBuffer = 1;

    pac193xErrorCode_t errorCode = pac193xInternalGetDataFromSensor(
        &info->product_id, sizeOfResponseBuffer, PAC193X_CMD_READ_PRODUCT_ID);
    if (errorCode != PAC193X_NO_ERROR) {
        info->product_id = 0;
        info->manufacturer_id = 0;
        info->revision_id = 0;
        return errorCode;
    }

    errorCode = pac193xInternalGetDataFromSensor(&info->manufacturer_id, sizeOfResponseBuffer,
                                                 PAC193X_CMD_READ_MANUFACTURER_ID);
    if (errorCode != PAC193X_NO_ERROR) {
        info->product_id = 0;
        info->manufacturer_id = 0;
        info->revision_id = 0;
        return errorCode;
    }

    errorCode = pac193xInternalGetDataFromSensor(&info->revision_id, sizeOfResponseBuffer,
                                                 PAC193X_CMD_READ_REVISION_ID);
    if (errorCode != PAC193X_NO_ERROR) {
        info->product_id = 0;
        info->manufacturer_id = 0;
        info->revision_id = 0;
        return errorCode;
    }

    return PAC193X_NO_ERROR;
}

pac193xErrorCode_t pac193xGetMeasurementForChannel(pac193xChannel_t channel,
                                                   pac193xValueToMeasure_t valueToMeasure,
                                                   float *value) {
    /* check if channel is activated */
    bool validChannel;
    switch (channel) {
    case (PAC193X_CHANNEL01):
        validChannel = pac193xSensorConfiguration.usedChannels.struct_channelsInUse.channel1 == 1;
        break;
    case (PAC193X_CHANNEL02):
        validChannel = pac193xSensorConfiguration.usedChannels.struct_channelsInUse.channel2 == 1;
        break;
    case (PAC193X_CHANNEL03):
        validChannel = pac193xSensorConfiguration.usedChannels.struct_channelsInUse.channel3 == 1;
        break;
    case (PAC193X_CHANNEL04):
        validChannel = pac193xSensorConfiguration.usedChannels.struct_channelsInUse.channel4 == 1;
        break;
    default:
        validChannel = false;
    }
    if (!validChannel) {
        return PAC193X_INVALID_CHANNEL;
    }

    /* store configurations for measurements */
    pac193xMeasurementProperties_t properties;
    pac193xErrorCode_t errorCode =
        pac193xInternalSetMeasurementProperties(&properties, valueToMeasure);
    if (errorCode != PAC193X_NO_ERROR) {
        return errorCode;
    }

    /* retrieve data from sensor */
    uint8_t responseBuffer[properties.sizeOfResponseBuffer];
    errorCode = pac193xInternalGetDataFromSensor(responseBuffer, properties.sizeOfResponseBuffer,
                                                 properties.startReadAddress);
    if (errorCode != PAC193X_NO_ERROR) {
        return errorCode;
    }

    /* transform raw data */
    uint64_t rawValue = pac193xInternalTransformResponseBufferToUInt64(
        responseBuffer, properties.sizeOfResponseBuffer);
    *value = (*properties.calculationFunction)(
        rawValue, pac193xInternalTranslateChannelToRSenseArrayIndex(channel));

    return errorCode;
}

pac193xErrorCode_t pac193xGetAllMeasurementsForChannel(pac193xChannel_t channel,
                                                       pac193xMeasurements_t *measurements) {
    /* refresh to get latest values */
    pac193xInternalRefreshV();

    pac193xErrorCode_t errorCode =
        pac193xGetMeasurementForChannel(channel, PAC193X_VSOURCE, &measurements->voltageSource);
    if (errorCode != PAC193X_NO_ERROR) {
        return errorCode;
    }

    errorCode =
        pac193xGetMeasurementForChannel(channel, PAC193X_VSENSE, &measurements->voltageSense);
    if (errorCode != PAC193X_NO_ERROR) {
        return errorCode;
    }

    errorCode = pac193xGetMeasurementForChannel(channel, PAC193X_ISENSE, &measurements->iSense);
    if (errorCode != PAC193X_NO_ERROR) {
        return errorCode;
    }

    errorCode =
        pac193xGetMeasurementForChannel(channel, PAC193X_PACTUAL, &measurements->powerActual);
    if (errorCode != PAC193X_NO_ERROR) {
        return errorCode;
    }

    errorCode = pac193xGetMeasurementForChannel(channel, PAC193X_ENERGY, &measurements->energy);

    return errorCode;
}

/* endregion */

/* region STATIC FUNCTION IMPLEMENTATIONS */

static pac193xErrorCode_t pac193xInternalRefresh(void) {
    PRINT_DEBUG("send refresh signal to sensor")
    /* trigger refresh */
    pac193xErrorCode_t errorCode = pac193xInternalSendRequestToSensor(PAC193X_CMD_REFRESH);
    if (errorCode != PAC193X_NO_ERROR) {
        PRINT_DEBUG("refresh send failed, error was %02X", errorCode)
        return errorCode;
    }

    /* sleep because sensor is unreachable for 1ms after refresh */
    sleep_ms(1);

    PRINT_DEBUG("refresh successful")
    return PAC193X_NO_ERROR;
}

static pac193xErrorCode_t pac193xInternalRefreshV(void) {
    PRINT_DEBUG("send pac193xInternalRefreshV signal to sensor")
    /* trigger refresh */
    pac193xErrorCode_t errorCode = pac193xInternalSendRequestToSensor(PAC193X_CMD_REFRESH_V);
    if (errorCode != PAC193X_NO_ERROR) {
        PRINT_DEBUG("pac193xInternalRefreshV send failed, error was %02X", errorCode)
        return errorCode;
    }

    /* sleep because sensor is unreachable for 1ms after refresh */
    sleep_ms(1);

    PRINT_DEBUG("pac193xInternalRefreshV successful")
    return PAC193X_NO_ERROR;
}

static uint8_t pac193xInternalTranslateChannelToRSenseArrayIndex(pac193xChannel_t channel) {
    uint8_t channelIndex;
    switch (channel) {
    case (PAC193X_CHANNEL01):
        channelIndex = 0;
        break;
    case (PAC193X_CHANNEL02):
        channelIndex = 1;
        break;
    case (PAC193X_CHANNEL03):
        channelIndex = 2;
        break;
    case (PAC193X_CHANNEL04):
        channelIndex = 3;
        break;
    default:
        channelIndex = -1;
    }

    PRINT_DEBUG("channel: %i; index: %i", channel, channelIndex)
    return channelIndex;
}

static pac193xErrorCode_t
pac193xInternalSetMeasurementProperties(pac193xMeasurementProperties_t *properties,
                                        pac193xValueToMeasure_t valueToMeasure) {
    PRINT_DEBUG("setting properties for requested measurement")
    /*! address of a value to be measured are separated by 0x01 per channel:
     *   -> CMD_READ_VBUS1 = 0x07, CMD_READ_VBUS2 = 0x08, ...
     * \Important actual address will be added later by
     * pac193xInternalSetMeasurementProperties(...) function
     */
    properties->startReadAddress = 1;

    switch (valueToMeasure) {
    case PAC193X_VSOURCE:
        properties->startReadAddress += PAC193X_CMD_READ_VBUS1;
        properties->calculationFunction = &pac193xInternalCalculateVoltageOfSource;
        properties->sizeOfResponseBuffer = 2;
        break;
    case PAC193X_VSOURCE_AVG:
        properties->startReadAddress += PAC193X_CMD_READ_VBUS1_AVG;
        properties->calculationFunction = &pac193xInternalCalculateVoltageOfSource;
        properties->sizeOfResponseBuffer = 2;
        break;
    case PAC193X_VSENSE:
        properties->startReadAddress += PAC193X_CMD_READ_VSENSE1;
        properties->calculationFunction = &pac193xInternalCalculateVoltageOfSense;
        properties->sizeOfResponseBuffer = 2;
        break;
    case PAC193X_VSENSE_AVG:
        properties->startReadAddress += PAC193X_CMD_READ_VSENSE1_AVG;
        properties->calculationFunction = &pac193xInternalCalculateVoltageOfSense;
        properties->sizeOfResponseBuffer = 2;
        break;
    case PAC193X_ISENSE:
        properties->startReadAddress += PAC193X_CMD_READ_VSENSE1;
        properties->calculationFunction = &pac193xInternalCalculateCurrentOfSense;
        properties->sizeOfResponseBuffer = 2;
        break;
    case PAC193X_ISENSE_AVG:
        properties->startReadAddress += PAC193X_CMD_READ_VSENSE1_AVG;
        properties->calculationFunction = &pac193xInternalCalculateCurrentOfSense;
        properties->sizeOfResponseBuffer = 2;
        break;
    case PAC193X_PACTUAL:
        properties->startReadAddress += PAC193X_CMD_READ_VPOWER1;
        properties->calculationFunction = &pac193xInternalCalculateActualPower;
        properties->sizeOfResponseBuffer = 4;
        break;
    case PAC193X_ENERGY:
        properties->startReadAddress += PAC193X_CMD_READ_VPOWER1_ACC;
        properties->calculationFunction = &pac193xInternalCalculateEnergy;
        properties->sizeOfResponseBuffer = 6;
        break;
    default:
        PRINT_DEBUG("invalid properties")
        return PAC193X_INVALID_MEASUREMENT;
    }

    PRINT_DEBUG("settings applied successful")
    return PAC193X_NO_ERROR;
}

static pac193xErrorCode_t
pac193xInternalSendConfigurationToSensor(pac193xRegisterAddress_t registerToWrite,
                                         pac193xSettings_t settingsToWrite) {
    uint8_t sizeOfCommandBuffer = 2;
    uint8_t commandBuffer[sizeOfCommandBuffer];
    commandBuffer[0] = registerToWrite;
    commandBuffer[1] = settingsToWrite;

    PRINT_DEBUG("send configuration to sensor")
    /* send new configuration to sensor */
    i2cErrorCode_t i2cErrorCode = i2cWriteCommand(commandBuffer, sizeOfCommandBuffer,
                                                  pac193xSensorConfiguration.i2c_slave_address,
                                                  pac193xSensorConfiguration.i2c_host);
    if (i2cErrorCode != I2C_NO_ERROR) {
        PRINT_DEBUG("send configuration failed, error was %02X", i2cErrorCode)
        return PAC193X_SEND_COMMAND_ERROR;
    }
    PRINT_DEBUG("configuration send successful")

    pac193xErrorCode_t errorCode = pac193xInternalRefreshV();
    if (errorCode != PAC193X_NO_ERROR) {
        return errorCode;
    }

    return PAC193X_NO_ERROR;
}

static pac193xErrorCode_t
pac193xInternalSendRequestToSensor(pac193xRegisterAddress_t registerToRead) {
    uint8_t sizeOfCommandBuffer = 1;
    uint8_t commandBuffer[sizeOfCommandBuffer];
    commandBuffer[0] = registerToRead;

    PRINT_DEBUG("request data from sensor")
    i2cErrorCode_t errorCode = i2cWriteCommand(commandBuffer, sizeOfCommandBuffer,
                                               pac193xSensorConfiguration.i2c_slave_address,
                                               pac193xSensorConfiguration.i2c_host);
    if (errorCode != I2C_NO_ERROR) {
        PRINT_DEBUG("sending request failed, error was %02X", errorCode)
        return PAC193X_SEND_COMMAND_ERROR;
    }

    return PAC193X_NO_ERROR;
}

static pac193xErrorCode_t pac193xInternalReceiveDataFromSensor(uint8_t *responseBuffer,
                                                               uint8_t sizeOfResponseBuffer) {
    PRINT_DEBUG("receiving data from sensor")
    i2cErrorCode_t errorCode = i2cReadData(responseBuffer, sizeOfResponseBuffer,
                                           pac193xSensorConfiguration.i2c_slave_address,
                                           pac193xSensorConfiguration.i2c_host);
    if (errorCode != I2C_NO_ERROR) {
        PRINT_DEBUG("receiving data failed, error was %02X", errorCode)
        return PAC193X_RECEIVE_DATA_ERROR;
    }
    PRINT_DEBUG("received data successful")

    return PAC193X_NO_ERROR;
}

static pac193xErrorCode_t
pac193xInternalGetDataFromSensor(uint8_t *responseBuffer, uint8_t sizeOfResponseBuffer,
                                 pac193xRegisterAddress_t registerToRead) {
    /* trigger refresh to store current values for request */
    pac193xErrorCode_t errorCode = pac193xInternalRefresh();
    if (errorCode != PAC193X_NO_ERROR) {
        return errorCode;
    }

    errorCode = pac193xInternalSendRequestToSensor(registerToRead);
    if (errorCode != PAC193X_NO_ERROR) {
        return errorCode;
    }

    errorCode = pac193xInternalReceiveDataFromSensor(responseBuffer, sizeOfResponseBuffer);
    if (errorCode != PAC193X_NO_ERROR) {
        return errorCode;
    }

    return PAC193X_NO_ERROR;
}

static uint64_t pac193xInternalTransformResponseBufferToUInt64(const uint8_t *responseBuffer,
                                                               uint8_t sizeOfResponseBuffer) {
    PRINT_DEBUG("transforming buffer to uint64")
    uint64_t scalar = responseBuffer[0];
    for (uint8_t index = 1; index < sizeOfResponseBuffer; index++) {
        scalar <<= 8;
        scalar |= (uint64_t)(responseBuffer[index]);
    }

    PRINT_DEBUG("output: %llu", scalar)
    return scalar;
}

static float pac193xInternalConvertToFloat(uint64_t input) {
    PRINT_DEBUG("converting value to float")
    PRINT_DEBUG("output: %f", ((float)input))
    return (float)input;
}

static float pac193xInternalCalculateVoltageOfSource(uint64_t input, uint8_t channel) {
    PRINT_DEBUG("calculating voltage of source")
    float vSource =
        32.0f * (pac193xInternalConvertToFloat(input) / pac193xInternalUnipolarVoltageDenominator);
    PRINT_DEBUG("output: %f", vSource)
    return vSource;
}

static float pac193xInternalCalculateVoltageOfSense(uint64_t input, uint8_t channel) {
    PRINT_DEBUG("calculating voltage of sense")
    float vSense =
        0.1f * (pac193xInternalConvertToFloat(input) / pac193xInternalUnipolarVoltageDenominator);
    PRINT_DEBUG("output: %f", vSense)
    return vSense;
}

static float pac193xInternalCalculateCurrentOfSense(uint64_t input, uint8_t channel) {
    PRINT_DEBUG("calculating current of sense")
    float fsc = 0.1f / pac193xSensorConfiguration.rSense[channel];
    float iSense =
        fsc * (pac193xInternalConvertToFloat(input) / pac193xInternalUnipolarVoltageDenominator);
    PRINT_DEBUG("output: %f", iSense)
    return iSense;
}

static float pac193xInternalCalculateActualPower(uint64_t input, uint8_t channel) {
    PRINT_DEBUG("calculating actual power")
    float powerFSR = 3.2f / pac193xSensorConfiguration.rSense[channel];
    float powerConversionFactor =
        pac193xInternalConvertToFloat(input) / pac193xInternalUnipolarPowerDenominator;
    float powerActual = powerFSR * powerConversionFactor;
    PRINT_DEBUG("output: %f", powerActual)
    return powerActual;
}

static float pac193xInternalCalculateEnergy(uint64_t input, uint8_t channel) {
    PRINT_DEBUG("calculating energy")
    float powerFSR = 3.2f / pac193xSensorConfiguration.rSense[channel];
    float energy = pac193xInternalConvertToFloat(input) * powerFSR /
                   (pac193xInternalEnergyDenominator * pac193xInternalSamplingRate);
    PRINT_DEBUG("output: %f", energy)
    return energy;
}

/* endregion */
