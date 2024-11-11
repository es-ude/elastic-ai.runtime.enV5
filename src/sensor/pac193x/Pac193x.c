#define SOURCE_FILE "PAC193X"

#include <stdbool.h>
#include <stdlib.h>

#include "CException.h"

#include "Common.h"
#include "Gpio.h"
#include "I2c.h"
#include "Pac193x.h"
#include "Pac193xInternal.h"
#include "Pac193xTypedefs.h"
#include "Sleep.h"

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

/* endregion CONSTANTS */

/* region HEADER FUNCTION IMPLEMENTATIONS */

/* region GENERAL FUNCTIONS */

void pac193xPowerUpSensor(pac193xSensorConfiguration_t sensor) {
    gpioErrorCode_t gpioErrorCode = gpioInitPin(sensor.powerPin, GPIO_OUTPUT);
    if (gpioErrorCode != GPIO_NO_ERROR) {
        Throw(PAC193X_INIT_ERROR);
    }

    gpioSetPin(sensor.powerPin, GPIO_PIN_LOW);

    /* sleep to make sure the sensor is idle  */
    sleep_for_ms(10);
}

void pac193xPowerDownSensor(pac193xSensorConfiguration_t sensor) {
    gpioErrorCode_t gpioErrorCode = gpioInitPin(sensor.powerPin, GPIO_OUTPUT);
    if (gpioErrorCode != GPIO_NO_ERROR) {
        Throw(PAC193X_INIT_ERROR);
    }

    gpioSetPin(sensor.powerPin, GPIO_PIN_HIGH);
}

void pac193xInit(pac193xSensorConfiguration_t sensor) {
    pac193xInternalCheckSensorAvailable(sensor);
    pac193xInternalSetDefaultConfiguration(sensor);
    pac193xRefreshData(sensor);
}

void pac193xSetChannelsInUse(pac193xSensorConfiguration_t sensor) {
    /* check if used channels are valid */
    if (sensor.usedChannels.uint_channelsInUse > 0b00010000) {
        Throw(PAC193X_INIT_ERROR);
    }

    /* send configuration to sensor */
    uint8_t channelsInUse = (sensor.usedChannels.uint_channelsInUse << 4) ^ 0xF0;
    pac193xInternalSendConfigurationToSensor(sensor, PAC193X_CMD_CHANNEL_DIS, channelsInUse);
    pac193xRefreshData(sensor);
}

void pac193xStartAccumulation(pac193xSensorConfiguration_t sensor) {
    /* samplerate = 1024, disable sleep, disable single-show-mode,
     * enable alert, enable overflow alert
     */
    pac193xInternalSendConfigurationToSensor(sensor, PAC193X_CMD_CTRL, 0b00001010);

    /* refresh sensor to apply changes */
    pac193xRefreshData(sensor);
}

void pac193XStopAccumulation(pac193xSensorConfiguration_t sensor) {
    pac193xInternalSetDefaultConfiguration(sensor);
    /* refresh sensor to apply changes */
    pac193xRefreshData(sensor);
}

void pac193xSetSampleRate(pac193xSensorConfiguration_t sensor, pac193xSampleRate_t sampleRate) {

    uint8_t ctrlRegister;
    pac193xInternalGetDataFromSensor(sensor, &ctrlRegister, 1, PAC193X_CMD_CTRL);
    ctrlRegister = (ctrlRegister & 0x3F) | (sampleRate << 6);
    pac193xInternalSendConfigurationToSensor(sensor, PAC193X_CMD_CTRL, ctrlRegister);
    sensor.sampleRate = sampleRate;
}

void pac193xRefreshData(pac193xSensorConfiguration_t sensor) {
    /* essentially performing a REFRESH_V command*/

    PRINT_DEBUG("send pac193xInternalRefreshV signal to sensor");
    pac193xInternalSendRequestToSensor(sensor, PAC193X_CMD_REFRESH_V);

    PRINT_DEBUG("pac193xInternalRefreshV successful");
}

void pac193xRefreshDataAndResetAccumulator(pac193xSensorConfiguration_t sensor) {
    /* essentially performing a REFRESH command*/

    PRINT_DEBUG("send refresh signal to sensor");
    pac193xInternalSendRequestToSensor(sensor, PAC193X_CMD_REFRESH);

    PRINT_DEBUG("refresh successful");
}
/* endregion GENERAL FUNCTIONS */

pac193xSensorId_t* pac193xGetSensorInfo(pac193xSensorConfiguration_t sensor, pac193xSensorId_t *info) {

    CEXCEPTION_T e;
    uint8_t sizeOfResponseBuffer = 1;

    /* needs to be called once before reading values to get the latest values*/
    pac193xRefreshData(sensor);

    Try {
        pac193xInternalGetDataFromSensor(sensor, &info->product_id, sizeOfResponseBuffer,
                                         PAC193X_CMD_READ_PRODUCT_ID);
        pac193xInternalGetDataFromSensor(sensor, &info->manufacturer_id, sizeOfResponseBuffer,
                                         PAC193X_CMD_READ_MANUFACTURER_ID);

        pac193xInternalGetDataFromSensor(sensor, &info->revision_id, sizeOfResponseBuffer,
                                         PAC193X_CMD_READ_REVISION_ID);
    }
    Catch(e) {
        info->product_id = 0;
        info->manufacturer_id = 0;
        info->revision_id = 0;
        Throw(e);
    }
    return info;
}

/* region READ MEASUREMENTS */

float* pac193xGetMeasurementForChannel(pac193xSensorConfiguration_t sensor, pac193xChannel_t channel,
                                     pac193xValueToMeasure_t valueToMeasure, float *value) {
    CEXCEPTION_T e;
    if (!pac193xInternalCheckChannelIsActive(sensor.usedChannels, channel)) {
        Throw(PAC193X_INVALID_CHANNEL);
    }

    Try {
        pac193xInternalGetData(sensor, channel, valueToMeasure, value);
    }
    Catch(e) {
        *value = 0;
        Throw(e);
    }

    return value;
}

pac193xMeasurements_t* pac193xGetMeasurementsForChannel(pac193xSensorConfiguration_t sensor, pac193xChannel_t channel,
                                      pac193xMeasurements_t *measurements) {
    CEXCEPTION_T e;
    if (!pac193xInternalCheckChannelIsActive(sensor.usedChannels, channel)) {
        measurements->voltageSource = 0;
        measurements->voltageSense = 0;
        measurements->currentSense = 0;
        measurements->power = 0;
        Throw(PAC193X_INVALID_CHANNEL);
    }

    Try {
        pac193xInternalGetData(sensor, channel, PAC193X_VSOURCE, &measurements->voltageSource);
        pac193xInternalGetData(sensor, channel, PAC193X_VSENSE, &measurements->voltageSense);
        pac193xInternalGetData(sensor, channel, PAC193X_CURRENT, &measurements->currentSense);
        pac193xInternalGetData(sensor, channel, PAC193X_POWER, &measurements->power);
    }
    Catch(e) {
        measurements->voltageSource = 0;
        measurements->voltageSense = 0;
        measurements->currentSense = 0;
        measurements->power = 0;
        Throw(e);
    }

    return measurements;
}

pac193xMeasurements_t* pac193xGetAveragesForChannel(pac193xSensorConfiguration_t sensor, pac193xChannel_t channel,
                                  pac193xMeasurements_t *measurements) {
    CEXCEPTION_T e;

    if (!pac193xInternalCheckChannelIsActive(sensor.usedChannels, channel)) {
        Throw(PAC193X_INVALID_CHANNEL);
    }

    Try {
        pac193xInternalGetData(sensor, channel, PAC193X_VSOURCE_AVG, &measurements->voltageSource);
        pac193xInternalGetData(sensor, channel, PAC193X_VSENSE_AVG, &measurements->voltageSense);
        pac193xInternalGetData(sensor, channel, PAC193X_CURRENT_AVG, &measurements->currentSense);
    }
    Catch(e) {
        measurements->voltageSource = 0;
        measurements->voltageSense = 0;
        measurements->currentSense = 0;
        measurements->power = 0;
        Throw(e);
    }

    /* no average for power => set to zero */
    measurements->power = 0;

    return measurements;
}

pac193xEnergyMeasurements_t* pac193xReadEnergyForAllChannels(pac193xSensorConfiguration_t sensor,
                                     pac193xEnergyMeasurements_t *measurements) {

    /* read values counter */
    uint8_t responseBuffer[28];
    pac193xInternalGetDataFromSensor(sensor, responseBuffer, 3, PAC193X_CMD_CTRL);

    measurements->overflow = responseBuffer[0] & 0x01;

    measurements->numberOfAccumulatedValues =
        pac193xInternalTransformResponseBufferToUInt64(&responseBuffer[1], 3);

    measurements->energyChannel1 = pac193xInternalCalculateEnergy(
        pac193xInternalTransformResponseBufferToUInt64(&responseBuffer[4], 6),
        sensor.rSense[pac193xInternalTranslateChannelToRSenseArrayIndex(PAC193X_CHANNEL01)],
        sensor.sampleRate);
    measurements->energyChannel2 = pac193xInternalCalculateEnergy(
        pac193xInternalTransformResponseBufferToUInt64(&responseBuffer[10], 6),
        sensor.rSense[pac193xInternalTranslateChannelToRSenseArrayIndex(PAC193X_CHANNEL02)],
        sensor.sampleRate);
    measurements->energyChannel3 = pac193xInternalCalculateEnergy(
        pac193xInternalTransformResponseBufferToUInt64(&responseBuffer[16], 6),
        sensor.rSense[pac193xInternalTranslateChannelToRSenseArrayIndex(PAC193X_CHANNEL03)],
        sensor.sampleRate);
    measurements->energyChannel4 = pac193xInternalCalculateEnergy(
        pac193xInternalTransformResponseBufferToUInt64(&responseBuffer[22], 6),
        sensor.rSense[pac193xInternalTranslateChannelToRSenseArrayIndex(PAC193X_CHANNEL04)],
        sensor.sampleRate);

    return measurements;
}

/* endregion READ MEASUREMENTS*/

/* endregion HEADER FUNCTION IMPLEMENTATIONS */

/* region STATIC FUNCTION IMPLEMENTATIONS */

static void pac193xInternalCheckSensorAvailable(pac193xSensorConfiguration_t sensor) {
    uint8_t sizeOfCommandBuffer = 1;
    uint8_t commandBuffer[sizeOfCommandBuffer];
    commandBuffer[0] = PAC193X_CMD_READ_MANUFACTURER_ID;

    i2cErrorCode_t i2CErrorCode = i2cWriteCommand(sensor.i2c_host, sensor.i2c_slave_address,
                                                  commandBuffer, sizeOfCommandBuffer);

    if (i2CErrorCode != I2C_NO_ERROR) {
        Throw(PAC193X_INIT_ERROR);
    }
}

static void pac193xInternalSetDefaultConfiguration(pac193xSensorConfiguration_t sensor) {
    pac193xSetChannelsInUse(sensor);

    /* enable single-shot mode, enable slow mode, disable alert/overflow pin */
    pac193xInternalSendConfigurationToSensor(sensor, PAC193X_CMD_CTRL, 0b11010000);
    sensor.sampleRate = PAC193X_8_SAMPLES_PER_SEC;

    /* sets measurement to unipolar mode */
    pac193xInternalSendConfigurationToSensor(sensor, PAC193X_CMD_NEG_PWR, 0b00000000);

    /* enable limited refresh function and set POR to 0 */
    pac193xInternalSendConfigurationToSensor(sensor, PAC193X_CMD_SLOW, 0b00010100);
}

static void pac193xInternalSendConfigurationToSensor(pac193xSensorConfiguration_t sensor,
                                                     pac193xRegisterAddress_t registerToWrite,
                                                     pac193xSettings_t settingsToWrite) {
    uint8_t sizeOfCommandBuffer = 2;
    uint8_t commandBuffer[sizeOfCommandBuffer];
    commandBuffer[0] = registerToWrite;
    commandBuffer[1] = settingsToWrite;

    PRINT_DEBUG("send configuration to sensor");
    i2cErrorCode_t i2cErrorCode = i2cWriteCommand(sensor.i2c_host, sensor.i2c_slave_address,
                                                  commandBuffer, sizeOfCommandBuffer);
    if (i2cErrorCode != I2C_NO_ERROR) {
        PRINT_DEBUG("send configuration failed, error was %02X", i2cErrorCode);
        Throw(PAC193X_SEND_COMMAND_ERROR);
    }
    PRINT_DEBUG("configuration send successful");
}

static void pac193xInternalSendRequestToSensor(pac193xSensorConfiguration_t sensor,
                                               pac193xRegisterAddress_t registerToRead) {
    uint8_t sizeOfCommandBuffer = 1;
    uint8_t commandBuffer[sizeOfCommandBuffer];
    commandBuffer[0] = registerToRead;

    PRINT_DEBUG("request data from sensor");
    i2cErrorCode_t errorCode = i2cWriteCommand(sensor.i2c_host, sensor.i2c_slave_address,
                                               commandBuffer, sizeOfCommandBuffer);
    if (errorCode != I2C_NO_ERROR) {
        PRINT_DEBUG("sending request failed, error was %02X", errorCode);
        Throw(PAC193X_SEND_COMMAND_ERROR);
    }
}

static void pac193xInternalReceiveDataFromSensor(pac193xSensorConfiguration_t sensor,
                                                 uint8_t *responseBuffer,
                                                 uint8_t sizeOfResponseBuffer) {
    PRINT_DEBUG("receiving data from sensor");
    i2cErrorCode_t errorCode = i2cReadData(sensor.i2c_host, sensor.i2c_slave_address,
                                           responseBuffer, sizeOfResponseBuffer);
    if (errorCode != I2C_NO_ERROR) {
        PRINT_DEBUG("receiving data failed, error was %02X", errorCode);
        Throw(PAC193X_RECEIVE_DATA_ERROR);
    }
    PRINT_DEBUG("received data successful");
}

static void pac193xInternalGetDataFromSensor(pac193xSensorConfiguration_t sensor,
                                             uint8_t *responseBuffer, uint8_t sizeOfResponseBuffer,
                                             pac193xRegisterAddress_t registerToRead) {
    pac193xInternalSendRequestToSensor(sensor, registerToRead);

    pac193xInternalReceiveDataFromSensor(sensor, responseBuffer, sizeOfResponseBuffer);
}

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
        channelIndex = UINT8_MAX;
    }

    PRINT_DEBUG("channel: %i; index: %i", channel, channelIndex);
    return channelIndex;
}

static void pac193xInternalSetMeasurementProperties(pac193xMeasurementProperties_t *properties,
                                                    pac193xValueToMeasure_t valueToMeasure) {
    PRINT_DEBUG("setting properties for requested measurement");

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
    case PAC193X_CURRENT:
        properties->startReadAddress += PAC193X_CMD_READ_VSENSE1;
        properties->calculationFunction = &pac193xInternalCalculateCurrentOfSense;
        properties->sizeOfResponseBuffer = 2;
        break;
    case PAC193X_POWER:
        properties->startReadAddress += PAC193X_CMD_READ_VPOWER1;
        properties->calculationFunction = &pac193xInternalCalculatePower;
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
    case PAC193X_CURRENT_AVG:
        properties->startReadAddress += PAC193X_CMD_READ_VSENSE1_AVG;
        properties->calculationFunction = &pac193xInternalCalculateCurrentOfSense;
        properties->sizeOfResponseBuffer = 2;
    case PAC193X_ENERGY:
        properties->startReadAddress += PAC193X_CMD_READ_VPOWER1_ACC;
        properties->calculationFunction = &pac193xInternalCalculateEnergy;
        properties->sizeOfResponseBuffer = 6;
        break;
    case PAC193X_ACCUMULATOR:
        properties->startReadAddress += PAC193X_CMD_READ_ACC_COUNT;
        properties->calculationFunction = &pac193xInternalCalculateAccumulatorCount;
        properties->sizeOfResponseBuffer = 3;
    default:
        PRINT_DEBUG("invalid properties");
        Throw(PAC193X_INVALID_MEASUREMENT);
    }

    PRINT_DEBUG("settings applied successful");
}

static void pac193xInternalGetData(pac193xSensorConfiguration_t sensor, pac193xChannel_t channel,
                                   pac193xValueToMeasure_t valueToMeasure, float *value) {
    /* store configurations for measurements */
    pac193xMeasurementProperties_t properties;
    /* set channel offset for property
     * address of a value to be measured are separated by 0x01 per channel:
     *   -> CMD_READ_VBUS1 = 0x07, CMD_READ_VBUS2 = 0x08, ...
     */
    properties.startReadAddress = channel;
    pac193xInternalSetMeasurementProperties(&properties, valueToMeasure);

    /* retrieve data from sensor */
    uint8_t responseBuffer[properties.sizeOfResponseBuffer];
    pac193xInternalGetDataFromSensor(sensor, responseBuffer, properties.sizeOfResponseBuffer,
                                     properties.startReadAddress);

    /* transform raw data */
    uint64_t rawValue = pac193xInternalTransformResponseBufferToUInt64(
        responseBuffer, properties.sizeOfResponseBuffer);
    *value = (*properties.calculationFunction)(
        rawValue, sensor.rSense[pac193xInternalTranslateChannelToRSenseArrayIndex(channel)],
        sensor.sampleRate);
}

static uint64_t pac193xInternalTransformResponseBufferToUInt64(const uint8_t *responseBuffer,
                                                               uint8_t sizeOfResponseBuffer) {
    PRINT_DEBUG("transforming buffer to uint64");
    pac193xInternalDataBuffer_t converter = {.value = 0};
    for (size_t index = 0; index < sizeOfResponseBuffer; index++) {
        converter.raw[sizeOfResponseBuffer - 1 - index] = responseBuffer[index];
    }

    PRINT_DEBUG("output: %llu", converter.value);
    return converter.value;
}

static float pac193xInternalConvertToFloat(uint64_t input) {
    PRINT_DEBUG("converting value to float");
    PRINT_DEBUG("output: %f", ((float)input));
    return (float)input;
}

static float pac193xInternalCalculateAccumulatorCount(uint64_t input,
                                                      __attribute((__unused__)) float resistor,
                                                      __attribute((__unused__))
                                                      uint8_t sampleRate) {
    return pac193xInternalConvertToFloat(input);
}

static float pac193xInternalCalculateVoltageOfSource(uint64_t input,
                                                     __attribute((__unused__)) float resistor,
                                                     __attribute((__unused__)) uint8_t sampleRate) {
    PRINT_DEBUG("calculating voltage of source");
    float vSource =
        32.0f * (pac193xInternalConvertToFloat(input) / pac193xInternalUnipolarVoltageDenominator);
    PRINT_DEBUG("output: %f", vSource);
    return vSource;
}

static float pac193xInternalCalculateVoltageOfSense(uint64_t input,
                                                    __attribute((__unused__)) float resistor,
                                                    __attribute((__unused__)) uint8_t sampleRate) {
    PRINT_DEBUG("calculating voltage of sense");
    float vSense =
        0.1f * (pac193xInternalConvertToFloat(input) / pac193xInternalUnipolarVoltageDenominator);
    PRINT_DEBUG("output: %f", vSense);
    return vSense;
}

static float pac193xInternalCalculateCurrentOfSense(uint64_t input, float resistor,
                                                    __attribute((__unused__)) uint8_t sampleRate) {
    PRINT_DEBUG("calculating current of sense");
    float fsc = 0.1f / resistor;
    float iSense =
        fsc * (pac193xInternalConvertToFloat(input) / pac193xInternalUnipolarVoltageDenominator);
    PRINT_DEBUG("output: %f", iSense);
    return iSense;
}

static float pac193xInternalCalculatePower(uint64_t input, float resistor,
                                           __attribute((__unused__)) uint8_t sampleRate) {
    PRINT_DEBUG("calculating power");
    float powerFSR = 3.2f / resistor;
    float powerConversionFactor =
        pac193xInternalConvertToFloat(input) / pac193xInternalUnipolarPowerDenominator;
    float powerActual = powerFSR * powerConversionFactor;
    PRINT_DEBUG("output: %f", powerActual);
    return powerActual;
}

static float pac193xInternalGetSampleRate(uint8_t sampleRate) {
    switch (sampleRate) {
    case PAC193X_1024_SAMPLES_PER_SEC:
        return 1024.0f;
    case PAC193X_256_SAMPLES_PER_SEC:
        return 256.0f;
    case PAC193X_64_SAMPLES_PER_SEC:
        return 64.0f;
    case PAC193X_8_SAMPLES_PER_SEC:
        return 8.0f;
    default:
        return 0.0f;
    }
}
static float pac193xInternalCalculateEnergy(uint64_t input, float resistor, uint8_t sampleRate) {
    PRINT_DEBUG("calculating energy");
    float powerFSR = 3.2f / resistor;
    float energy = pac193xInternalConvertToFloat(input) * powerFSR /
                   (pac193xInternalEnergyDenominator * pac193xInternalGetSampleRate(sampleRate));
    PRINT_DEBUG("output: %f", energy);
    return energy;
}

/* endregion STATIC FUNCTION IMPLEMENTATIONS*/
