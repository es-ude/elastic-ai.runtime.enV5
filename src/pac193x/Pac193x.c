#define SOURCE_FILE "src/pac193x/pac193x.c"

#include "Pac193x.h"
#include "Common.h"
#include "Gpio.h"
#include "I2c.h"
#include "Sleep.h"
#include "include/Pac193x.h"
#include "include/Pac193xTypedefs.h"
#include <stdbool.h>
#include <stdlib.h>

/* Datasheet:
 * https://ww1.microchip.com/downloads/en/DeviceDoc/PAC1931-Family-Data-Sheet-DS20005850E.pdf
 */

/* region CONSTANTS */

/*! Denominator for unipolar voltage measurement: 2^{16} = 65536 */
static const float pac193xInternalUnipolarVoltageDenominator = (float)(1U << 16);

/*! Denominator for unipolar power measurement: 2^{32} = 4294967296
 *
 * \Information This denominator is 2^{28} according to the datasheet,
 *              however, testing has shown that 2^{32} is actually correct
 */
static const float pac193xInternalUnipolarPowerDenominator = (float)(1ULL << 32);

/*! Denominator for energy measurement: 2^28 = 268435456 */
static const float pac193xInternalEnergyDenominator = (float)(1ULL << 28);

/*! rate for samples per second#
 *
 * \Important Must be set using the ctrl register */
static const float pac193xInternalSamplingRate = 1024;

/* endregion */

/* region HEADER FUNCTION IMPLEMENTATIONS */

/* region GENERAL FUNCTIONS */

pac193xErrorCode_t pac193xPowerUpSensor(pac193xSensorConfiguration_t sensor) {
    gpioErrorCode_t gpioErrorCode = gpioEnablePin(sensor.powerPin, GPIO_POWER_UP);
    if (gpioErrorCode != GPIO_NO_ERROR) {
        return PAC193X_INIT_ERROR;
    }

    /* sleep to make sure the sensor is idle  */
    sleep_for_ms(10);

    return PAC193X_NO_ERROR;
}

pac193xErrorCode_t pac193xPowerDownSensor(pac193xSensorConfiguration_t sensor) {
    gpioErrorCode_t gpioErrorCode = gpioEnablePin(sensor.powerPin, GPIO_POWER_DOWN);
    if (gpioErrorCode != GPIO_NO_ERROR) {
        return PAC193X_INIT_ERROR;
    }

    return PAC193X_NO_ERROR;
}

pac193xErrorCode_t pac193xInit(pac193xSensorConfiguration_t sensor) {
    i2cInit(sensor.i2c_host, (100 * 1000), 6, 7);

    if (PAC193X_NO_ERROR != pac193xInternalCheckSensorAvailable(sensor)) {
        return PAC193X_INIT_ERROR;
    }

    pac193xErrorCode_t errorCode = pac193xInternalSetDefaultConfiguration(sensor);
    if (errorCode != PAC193X_NO_ERROR) {
        return PAC193X_INIT_ERROR;
    }

    return pac193xInternalRefresh(sensor);
}

pac193xErrorCode_t pac193xSetChannelsInUse(pac193xSensorConfiguration_t sensor) {
    /* check if used channels are valid */
    if (sensor.usedChannels.uint_channelsInUse > 0b00010000) {
        return PAC193X_INIT_ERROR;
    }

    /* send configuration to sensor */
    uint8_t channelsInUse = (sensor.usedChannels.uint_channelsInUse << 4) ^ 0xF0;
    pac193xErrorCode_t errorCode =
        pac193xInternalSendConfigurationToSensor(sensor, PAC193X_CMD_CHANNEL_DIS, channelsInUse);
    if (errorCode != PAC193X_NO_ERROR) {
        return PAC193X_INIT_ERROR;
    }

    return pac193xInternalRefresh(sensor);
}

/* endregion GENERAL FUNCTIONS */

/* region SINGLE SHOT MEASUREMENTS */

pac193xErrorCode_t pac193xGetSensorInfo(pac193xSensorConfiguration_t sensor,
                                        pac193xSensorId_t *info) {
    uint8_t sizeOfResponseBuffer = 1;

    /* needs to be called once before reading values to get the latest values*/
    pac193xErrorCode_t errorCode = pac193xInternalRefreshV(sensor);
    if (errorCode != PAC193X_NO_ERROR) {
        return errorCode;
    }

    errorCode = pac193xInternalGetDataFromSensor(sensor, &info->product_id, sizeOfResponseBuffer,
                                                 PAC193X_CMD_READ_PRODUCT_ID);
    if (errorCode != PAC193X_NO_ERROR) {
        info->product_id = 0;
        info->manufacturer_id = 0;
        info->revision_id = 0;
        return errorCode;
    }

    errorCode = pac193xInternalGetDataFromSensor(
        sensor, &info->manufacturer_id, sizeOfResponseBuffer, PAC193X_CMD_READ_MANUFACTURER_ID);
    if (errorCode != PAC193X_NO_ERROR) {
        info->product_id = 0;
        info->manufacturer_id = 0;
        info->revision_id = 0;
        return errorCode;
    }

    errorCode = pac193xInternalGetDataFromSensor(sensor, &info->revision_id, sizeOfResponseBuffer,
                                                 PAC193X_CMD_READ_REVISION_ID);
    if (errorCode != PAC193X_NO_ERROR) {
        info->product_id = 0;
        info->manufacturer_id = 0;
        info->revision_id = 0;
        return errorCode;
    }

    return PAC193X_NO_ERROR;
}

pac193xErrorCode_t pac193xGetMeasurementForChannel(pac193xSensorConfiguration_t sensor,
                                                   pac193xChannel_t channel,
                                                   pac193xValueToMeasure_t valueToMeasure,
                                                   float *value) {
    if (!pac193xInternalCheckChannelIsActive(sensor.usedChannels, channel)) {
        return PAC193X_INVALID_CHANNEL;
    }

    /* needs to be called once before reading values to get the latest values*/
    pac193xErrorCode_t errorCode = pac193xInternalRefreshV(sensor);
    if (errorCode != PAC193X_NO_ERROR) {
        return errorCode;
    }

    errorCode = pac193xInternalGetData(sensor, channel, valueToMeasure, value);
    if (errorCode != PAC193X_NO_ERROR) {
        *value = 0;
        return errorCode;
    }

    return PAC193X_NO_ERROR;
}

pac193xErrorCode_t pac193xGetAllMeasurementsForChannel(pac193xSensorConfiguration_t sensor,
                                                       pac193xChannel_t channel,
                                                       pac193xMeasurements_t *measurements) {
    if (!pac193xInternalCheckChannelIsActive(sensor.usedChannels, channel)) {
        return PAC193X_INVALID_CHANNEL;
    }

    /* needs to be called once before reading values to get the latest values*/
    pac193xErrorCode_t errorCode = pac193xInternalRefreshV(sensor);
    if (errorCode != PAC193X_NO_ERROR) {
        return errorCode;
    }

    errorCode =
        pac193xInternalGetData(sensor, channel, PAC193X_VSOURCE, &measurements->voltageSource);
    if (errorCode != PAC193X_NO_ERROR) {
        measurements->voltageSource = 0;
        measurements->voltageSense = 0;
        measurements->iSense = 0;
        measurements->powerActual = 0;
        return errorCode;
    }

    errorCode =
        pac193xInternalGetData(sensor, channel, PAC193X_VSENSE, &measurements->voltageSense);
    if (errorCode != PAC193X_NO_ERROR) {
        measurements->voltageSource = 0;
        measurements->voltageSense = 0;
        measurements->iSense = 0;
        measurements->powerActual = 0;
        return errorCode;
    }

    errorCode = pac193xInternalGetData(sensor, channel, PAC193X_ISENSE, &measurements->iSense);
    if (errorCode != PAC193X_NO_ERROR) {
        measurements->voltageSource = 0;
        measurements->voltageSense = 0;
        measurements->iSense = 0;
        measurements->powerActual = 0;
        return errorCode;
    }

    errorCode = pac193xInternalGetData(sensor, channel, PAC193X_POWER, &measurements->powerActual);
    if (errorCode != PAC193X_NO_ERROR) {
        measurements->voltageSource = 0;
        measurements->voltageSense = 0;
        measurements->iSense = 0;
        measurements->powerActual = 0;
        return errorCode;
    }

    return PAC193X_NO_ERROR;
}

/* endregion SINGLE SHOT MEASUREMENTS */

/* region CONTINUOUS MEASUREMENTS */

pac193xErrorCode_t pac193xStartAccumulation(pac193xSensorConfiguration_t sensor) {
    /* disable single shot mode, set sample rate to 1024 samples/s, enable overflow alert
     * enable alert pin -> not floating => if disable only 8 samples/s*/
    pac193xErrorCode_t errorCode =
        pac193xInternalSendConfigurationToSensor(sensor, PAC193X_CMD_CTRL, 0b00001010);
    if (errorCode != PAC193X_NO_ERROR) {
        return PAC193X_INIT_ERROR;
    }

    /* refresh sensor to apply changes */
    errorCode = pac193xInternalRefresh(sensor);
    if (errorCode != PAC193X_NO_ERROR) {
        return PAC193X_INIT_ERROR;
    }

    return PAC193X_NO_ERROR;
}

pac193xErrorCode_t pac193XStopAccumulation(pac193xSensorConfiguration_t sensor) {
    pac193xErrorCode_t errorCode = pac193xInternalSetDefaultConfiguration(sensor);
    if (errorCode != PAC193X_NO_ERROR) {
        return errorCode;
    }

    /* refresh sensor to apply changes */
    errorCode = pac193xInternalRefresh(sensor);
    if (errorCode != PAC193X_NO_ERROR) {
        return errorCode;
    }

    return PAC193X_NO_ERROR;
}

pac193xErrorCode_t
pac193xReadAccumulatedPowerForAllChannels(pac193xSensorConfiguration_t sensor,
                                          pac193xPowerMeasurements_t *measurements) {
    /* needs to be called once before reading values to get the latest values*/
    pac193xErrorCode_t errorCode = pac193xInternalRefresh(sensor);
    if (errorCode != PAC193X_NO_ERROR) {
        return errorCode;
    }

    // read accumulation counter (PAC193X_CMD_READ_ACC_COUNT)
    uint8_t *responseBuffer = malloc(3);
    errorCode =
        pac193xInternalGetDataFromSensor(sensor, responseBuffer, 3, PAC193X_CMD_READ_ACC_COUNT);
    if (errorCode != PAC193X_NO_ERROR) {
        free(responseBuffer);
        return PAC193X_RECEIVE_DATA_ERROR;
    }
    measurements->counterOfMeasurements =
        (uint32_t)pac193xInternalTransformResponseBufferToUInt64(responseBuffer, 3);
    free(responseBuffer);

    // read accumulated power value ( PAC193X_CMD_READ_VPOWER{1,2,3,4}_ACC )
    /* 6 bytes for each of the 4 channels */
    responseBuffer = malloc(24);
    errorCode =
        pac193xInternalGetDataFromSensor(sensor, responseBuffer, 24, PAC193X_CMD_READ_VPOWER1_ACC);
    if (errorCode != PAC193X_NO_ERROR) {
        free(responseBuffer);
        return PAC193X_RECEIVE_DATA_ERROR;
    }
    measurements->powerChannel1 = pac193xInternalCalculateEnergy(
        pac193xInternalTransformResponseBufferToUInt64(responseBuffer, 6),
        sensor.rSense[pac193xInternalTranslateChannelToRSenseArrayIndex(PAC193X_CHANNEL01)]);
    measurements->powerChannel2 = pac193xInternalCalculateEnergy(
        pac193xInternalTransformResponseBufferToUInt64(responseBuffer + 6, 6),
        sensor.rSense[pac193xInternalTranslateChannelToRSenseArrayIndex(PAC193X_CHANNEL02)]);
    measurements->powerChannel3 = pac193xInternalCalculateEnergy(
        pac193xInternalTransformResponseBufferToUInt64(responseBuffer + 12, 6),
        sensor.rSense[pac193xInternalTranslateChannelToRSenseArrayIndex(PAC193X_CHANNEL03)]);
    measurements->powerChannel4 = pac193xInternalCalculateEnergy(
        pac193xInternalTransformResponseBufferToUInt64(responseBuffer + 18, 6),
        sensor.rSense[pac193xInternalTranslateChannelToRSenseArrayIndex(PAC193X_CHANNEL04)]);
    free(responseBuffer);

    return pac193xInternalRefresh(sensor);
}

pac193xErrorCode_t pac193xReadAverageMeasurement(pac193xSensorConfiguration_t sensor,
                                                 pac193xChannel_t channel,
                                                 pac193xValueToMeasure_t valueToMeasure,
                                                 float *value) {
    if (!pac193xInternalCheckChannelIsActive(sensor.usedChannels, channel)) {
        return PAC193X_INVALID_CHANNEL;
    }

    /* needs to be called before reading values to get the latest values*/
    pac193xErrorCode_t errorCode = pac193xInternalRefreshV(sensor);
    if (errorCode != PAC193X_NO_ERROR) {
        return errorCode;
    }

    errorCode = pac193xInternalGetData(sensor, channel, valueToMeasure, value);
    if (errorCode != PAC193X_NO_ERROR) {
        *value = 0;
        return errorCode;
    }

    return PAC193X_NO_ERROR;
}

pac193xErrorCode_t
pac193xReadAllAverageMeasurementsForChannel(pac193xSensorConfiguration_t sensor,
                                            pac193xChannel_t channel,
                                            pac193xMeasurements_t *measurements) {
    if (!pac193xInternalCheckChannelIsActive(sensor.usedChannels, channel)) {
        return PAC193X_INVALID_CHANNEL;
    }

    /* needs to be called once before reading values to get the latest values*/
    pac193xErrorCode_t errorCode = pac193xInternalRefreshV(sensor);
    if (errorCode != PAC193X_NO_ERROR) {
        return errorCode;
    }

    errorCode =
        pac193xInternalGetData(sensor, channel, PAC193X_VSOURCE_AVG, &measurements->voltageSource);
    if (errorCode != PAC193X_NO_ERROR) {
        measurements->voltageSource = 0;
        measurements->voltageSense = 0;
        measurements->iSense = 0;
        measurements->powerActual = 0;
        return errorCode;
    }

    errorCode =
        pac193xInternalGetData(sensor, channel, PAC193X_VSENSE_AVG, &measurements->voltageSense);
    if (errorCode != PAC193X_NO_ERROR) {
        measurements->voltageSource = 0;
        measurements->voltageSense = 0;
        measurements->iSense = 0;
        measurements->powerActual = 0;
        return errorCode;
    }

    errorCode = pac193xInternalGetData(sensor, channel, PAC193X_ISENSE_AVG, &measurements->iSense);
    if (errorCode != PAC193X_NO_ERROR) {
        measurements->voltageSource = 0;
        measurements->voltageSense = 0;
        measurements->iSense = 0;
        measurements->powerActual = 0;
        return errorCode;
    }

    /* no average for power => set to zero */
    measurements->powerActual = 0;

    return PAC193X_NO_ERROR;
}

/* endregion */

/* region STATIC FUNCTION IMPLEMENTATIONS */

static pac193xErrorCode_t pac193xInternalCheckSensorAvailable(pac193xSensorConfiguration_t sensor) {
    uint8_t sizeOfCommandBuffer = 1;
    uint8_t commandBuffer[sizeOfCommandBuffer];
    commandBuffer[0] = PAC193X_CMD_READ_MANUFACTURER_ID;

    i2cErrorCode_t i2CErrorCode = i2cWriteCommand(commandBuffer, sizeOfCommandBuffer,
                                                  sensor.i2c_slave_address, sensor.i2c_host);

    if (i2CErrorCode != I2C_NO_ERROR) {
        return PAC193X_INIT_ERROR;
    }
    return PAC193X_NO_ERROR;
}

static pac193xErrorCode_t
pac193xInternalSetDefaultConfiguration(pac193xSensorConfiguration_t sensor) {
    pac193xErrorCode_t errorCode = pac193xSetChannelsInUse(sensor);
    if (errorCode != PAC193X_NO_ERROR) {
        return PAC193X_INIT_ERROR;
    }

    /* enable single-shot mode, disables alert/overflow pin */
    errorCode = pac193xInternalSendConfigurationToSensor(sensor, PAC193X_CMD_CTRL, 0b00010000);
    if (errorCode != PAC193X_NO_ERROR) {
        return PAC193X_INIT_ERROR;
    }

    /* sets measurement to unipolar mode */
    errorCode = pac193xInternalSendConfigurationToSensor(sensor, PAC193X_CMD_NEG_PWR, 0b00000000);
    if (errorCode != PAC193X_NO_ERROR) {
        return PAC193X_INIT_ERROR;
    }

    /* enable limited refresh function and set POR to 0 */
    errorCode = pac193xInternalSendConfigurationToSensor(sensor, PAC193X_CMD_SLOW, 0b00010100);
    if (errorCode != PAC193X_NO_ERROR) {
        return PAC193X_INIT_ERROR;
    }

    return PAC193X_NO_ERROR;
}

static pac193xErrorCode_t
pac193xInternalSendConfigurationToSensor(pac193xSensorConfiguration_t sensor,
                                         pac193xRegisterAddress_t registerToWrite,
                                         pac193xSettings_t settingsToWrite) {
    uint8_t sizeOfCommandBuffer = 2;
    uint8_t commandBuffer[sizeOfCommandBuffer];
    commandBuffer[0] = registerToWrite;
    commandBuffer[1] = settingsToWrite;

    PRINT_DEBUG("send configuration to sensor")
    i2cErrorCode_t i2cErrorCode = i2cWriteCommand(commandBuffer, sizeOfCommandBuffer,
                                                  sensor.i2c_slave_address, sensor.i2c_host);
    if (i2cErrorCode != I2C_NO_ERROR) {
        PRINT_DEBUG("send configuration failed, error was %02X", i2cErrorCode)
        return PAC193X_SEND_COMMAND_ERROR;
    }
    PRINT_DEBUG("configuration send successful")

    return PAC193X_NO_ERROR;
}

static pac193xErrorCode_t pac193xInternalRefresh(pac193xSensorConfiguration_t sensor) {
    PRINT_DEBUG("send refresh signal to sensor")
    pac193xErrorCode_t errorCode = pac193xInternalSendRequestToSensor(sensor, PAC193X_CMD_REFRESH);
    if (errorCode != PAC193X_NO_ERROR) {
        PRINT_DEBUG("refresh send failed, error was %02X", errorCode)
        return errorCode;
    }

    /* sleep because the sensor is unreachable for 1ms after refresh */
    sleep_for_ms(1);

    PRINT_DEBUG("refresh successful")
    return PAC193X_NO_ERROR;
}

static pac193xErrorCode_t pac193xInternalRefreshV(pac193xSensorConfiguration_t sensor) {
    PRINT_DEBUG("send pac193xInternalRefreshV signal to sensor")
    pac193xErrorCode_t errorCode =
        pac193xInternalSendRequestToSensor(sensor, PAC193X_CMD_REFRESH_V);
    if (errorCode != PAC193X_NO_ERROR) {
        PRINT_DEBUG("pac193xInternalRefreshV send failed, error was %02X", errorCode)
        return errorCode;
    }

    /* sleep because the sensor is unreachable for 1ms after refresh */
    sleep_for_ms(1);

    PRINT_DEBUG("pac193xInternalRefreshV successful")
    return PAC193X_NO_ERROR;
}

static pac193xErrorCode_t
pac193xInternalSendRequestToSensor(pac193xSensorConfiguration_t sensor,
                                   pac193xRegisterAddress_t registerToRead) {
    uint8_t sizeOfCommandBuffer = 1;
    uint8_t commandBuffer[sizeOfCommandBuffer];
    commandBuffer[0] = registerToRead;

    PRINT_DEBUG("request data from sensor")
    i2cErrorCode_t errorCode = i2cWriteCommand(commandBuffer, sizeOfCommandBuffer,
                                               sensor.i2c_slave_address, sensor.i2c_host);
    if (errorCode != I2C_NO_ERROR) {
        PRINT_DEBUG("sending request failed, error was %02X", errorCode)
        return PAC193X_SEND_COMMAND_ERROR;
    }

    return PAC193X_NO_ERROR;
}

static pac193xErrorCode_t pac193xInternalReceiveDataFromSensor(pac193xSensorConfiguration_t sensor,
                                                               uint8_t *responseBuffer,
                                                               uint8_t sizeOfResponseBuffer) {
    PRINT_DEBUG("receiving data from sensor")
    i2cErrorCode_t errorCode = i2cReadData(responseBuffer, sizeOfResponseBuffer,
                                           sensor.i2c_slave_address, sensor.i2c_host);
    if (errorCode != I2C_NO_ERROR) {
        PRINT_DEBUG("receiving data failed, error was %02X", errorCode)
        return PAC193X_RECEIVE_DATA_ERROR;
    }
    PRINT_DEBUG("received data successful")

    return PAC193X_NO_ERROR;
}

static pac193xErrorCode_t
pac193xInternalGetDataFromSensor(pac193xSensorConfiguration_t sensor, uint8_t *responseBuffer,
                                 uint8_t sizeOfResponseBuffer,
                                 pac193xRegisterAddress_t registerToRead) {
    pac193xErrorCode_t errorCode = pac193xInternalSendRequestToSensor(sensor, registerToRead);
    if (errorCode != PAC193X_NO_ERROR) {
        return errorCode;
    }

    return pac193xInternalReceiveDataFromSensor(sensor, responseBuffer, sizeOfResponseBuffer);
}

// ----------

static bool pac193xInternalCheckChannelIsActive(pac193xUsedChannels_t usedChannels,
                                                pac193xChannel_t channelToTest) {
    bool validChannel;
    switch (channelToTest) {
    case (PAC193X_CHANNEL01):
        validChannel = usedChannels.struct_channelsInUse.channel1 == 1;
        break;
    case (PAC193X_CHANNEL02):
        validChannel = usedChannels.struct_channelsInUse.channel2 == 1;
        break;
    case (PAC193X_CHANNEL03):
        validChannel = usedChannels.struct_channelsInUse.channel3 == 1;
        break;
    case (PAC193X_CHANNEL04):
        validChannel = usedChannels.struct_channelsInUse.channel4 == 1;
        break;
    default:
        validChannel = false;
    }
    return validChannel;
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

    switch (valueToMeasure) {
    case PAC193X_VSOURCE:
        properties->startReadAddress += PAC193X_CMD_READ_VBUS1;
        properties->calculationFunction = &pac193xInternalCalculateVoltageOfSource;
        properties->sizeOfResponseBuffer = 2;
        break;
    case PAC193X_VSENSE:
        properties->startReadAddress += PAC193X_CMD_READ_VSENSE1;
        properties->calculationFunction = &pac193xInternalCalculateVoltageOfSense;
        properties->sizeOfResponseBuffer = 2;
        break;
    case PAC193X_ISENSE:
        properties->startReadAddress += PAC193X_CMD_READ_VSENSE1;
        properties->calculationFunction = &pac193xInternalCalculateCurrentOfSense;
        properties->sizeOfResponseBuffer = 2;
        break;
    case PAC193X_POWER:
        properties->startReadAddress += PAC193X_CMD_READ_VPOWER1;
        properties->calculationFunction = &pac193xInternalCalculateActualPower;
        properties->sizeOfResponseBuffer = 4;
        break;
    case PAC193X_VSOURCE_AVG:
        properties->startReadAddress += PAC193X_CMD_READ_VBUS1_AVG;
        properties->calculationFunction = &pac193xInternalCalculateVoltageOfSource;
        properties->sizeOfResponseBuffer = 2;
        break;
    case PAC193X_VSENSE_AVG:
        properties->startReadAddress += PAC193X_CMD_READ_VSENSE1_AVG;
        properties->calculationFunction = &pac193xInternalCalculateVoltageOfSense;
        properties->sizeOfResponseBuffer = 2;
        break;
    case PAC193X_ISENSE_AVG:
        properties->startReadAddress += PAC193X_CMD_READ_VSENSE1_AVG;
        properties->calculationFunction = &pac193xInternalCalculateCurrentOfSense;
        properties->sizeOfResponseBuffer = 2;
        break;
    default:
        PRINT_DEBUG("invalid properties")
        return PAC193X_INVALID_MEASUREMENT;
    }

    PRINT_DEBUG("settings applied successful")
    return PAC193X_NO_ERROR;
}

static pac193xErrorCode_t pac193xInternalGetData(pac193xSensorConfiguration_t sensor,
                                                 pac193xChannel_t channel,
                                                 pac193xValueToMeasure_t valueToMeasure,
                                                 float *value) {
    /* store configurations for measurements */
    pac193xMeasurementProperties_t properties;
    /* set channel offset for property
     * address of a value to be measured are separated by 0x01 per channel:
     *   -> CMD_READ_VBUS1 = 0x07, CMD_READ_VBUS2 = 0x08, ...
     */
    properties.startReadAddress = channel;
    pac193xErrorCode_t errorCode =
        pac193xInternalSetMeasurementProperties(&properties, valueToMeasure);
    if (errorCode != PAC193X_NO_ERROR) {
        return errorCode;
    }

    /* retrieve data from sensor */
    uint8_t *responseBuffer = malloc(properties.sizeOfResponseBuffer);
    errorCode = pac193xInternalGetDataFromSensor(
        sensor, responseBuffer, properties.sizeOfResponseBuffer, properties.startReadAddress);
    if (errorCode != PAC193X_NO_ERROR) {
        free(responseBuffer);
        return errorCode;
    }

    /* transform raw data */
    uint64_t rawValue = pac193xInternalTransformResponseBufferToUInt64(
        responseBuffer, properties.sizeOfResponseBuffer);
    *value = (*properties.calculationFunction)(
        rawValue, sensor.rSense[pac193xInternalTranslateChannelToRSenseArrayIndex(channel)]);

    free(responseBuffer);
    return PAC193X_NO_ERROR;
}

// ----------

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

static float pac193xInternalCalculateVoltageOfSource(uint64_t input,
                                                     __attribute__((unused)) float resistor) {
    PRINT_DEBUG("calculating voltage of source")
    float vSource =
        32.0f * (pac193xInternalConvertToFloat(input) / pac193xInternalUnipolarVoltageDenominator);
    PRINT_DEBUG("output: %f", vSource)
    return vSource;
}

static float pac193xInternalCalculateVoltageOfSense(uint64_t input,
                                                    __attribute__((unused)) float resistor) {
    PRINT_DEBUG("calculating voltage of sense")
    float vSense =
        0.1f * (pac193xInternalConvertToFloat(input) / pac193xInternalUnipolarVoltageDenominator);
    PRINT_DEBUG("output: %f", vSense)
    return vSense;
}

static float pac193xInternalCalculateCurrentOfSense(uint64_t input, float resistor) {
    PRINT_DEBUG("calculating current of sense")
    float fsc = 0.1f / resistor;
    float iSense =
        fsc * (pac193xInternalConvertToFloat(input) / pac193xInternalUnipolarVoltageDenominator);
    PRINT_DEBUG("output: %f", iSense)
    return iSense;
}

static float pac193xInternalCalculateActualPower(uint64_t input, float resistor) {
    PRINT_DEBUG("calculating actual power")
    float powerFSR = 3.2f / resistor;
    float powerConversionFactor =
        pac193xInternalConvertToFloat(input) / pac193xInternalUnipolarPowerDenominator;
    float powerActual = powerFSR * powerConversionFactor;
    PRINT_DEBUG("output: %f", powerActual)
    return powerActual;
}

static float pac193xInternalCalculateEnergy(uint64_t input, float resistor) {
    PRINT_DEBUG("calculating energy")
    float powerFSR = 3.2f / resistor;
    float energy = pac193xInternalConvertToFloat(input) * powerFSR /
                   (pac193xInternalEnergyDenominator * pac193xInternalSamplingRate);
    PRINT_DEBUG("output: %f", energy)
    return energy;
}

/* endregion */
