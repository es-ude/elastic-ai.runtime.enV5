#define SOURCE_FILE "src/pac193x/pac193x.c"

#include <stdbool.h>
#include "pico/time.h"
#include "pac193x_public.h"
#include "pac193x_internal.h"
#include "gpio.h"
#include "i2c.h"
#include "common.h"

/* region HEADER FUNCTION IMPLEMENTATIONS */

pac193x_errorCode pac193x_powerUpSensor(void) {
    GPIO_ErrorCode errorCode = GPIO_setPower(29, GPIO_POWER_UP);
    if (errorCode != GPIO_NO_ERROR) {
        return PAC193X_INIT_ERROR;
    }

    return PAC193X_NO_ERROR;
}

pac193x_errorCode pac193x_powerDownSensor(void) {
    GPIO_ErrorCode errorCode = GPIO_setPower(29, GPIO_POWER_DOWN);
    if (errorCode != GPIO_NO_ERROR) {
        return PAC193X_INIT_ERROR;
    }

    return PAC193X_NO_ERROR;
}

pac193x_errorCode pac193x_init(i2c_inst_t *i2cHost, float resistanceValues[4], pac193x_usedChannels usedChannels) {
    /* save i2c access for later usage */
    sensorConfiguration.i2c_host = i2cHost;
    sensorConfiguration.i2c_slave_address = 0x11;

    /* power up sensor */
    pac193x_errorCode errorCode = pac193x_powerUpSensor();
    if (errorCode != PAC193X_NO_ERROR) {
        return PAC193X_INIT_ERROR;
    }

    /* sleep to make sure the sensor is fully initialized  */
    sleep_ms(10);

    I2C_Init(sensorConfiguration.i2c_host, (100 * 1000), 6, 7);

    /* check if pac193x is available on bus */
    uint8_t sizeOfCommandBuffer = 1;
    uint8_t commandBuffer[sizeOfCommandBuffer];
    commandBuffer[0] = PAC193X_CMD_READ_MANUFACTURER_ID;

    /* if i2c returns error -> sensor not available on bus */
    I2C_ErrorCode i2CErrorCode = I2C_WriteCommand(commandBuffer, sizeOfCommandBuffer,
                                                  sensorConfiguration.i2c_slave_address, sensorConfiguration.i2c_host
    );
    if (i2CErrorCode != I2C_NO_ERROR) {
        return PAC193X_INIT_ERROR;
    }

    errorCode = pac193x_setResistanceAtSense(resistanceValues);
    if (errorCode != PAC193X_NO_ERROR) {
        return PAC193X_INIT_ERROR;
    }

    errorCode = pac193x_setChannelsInUse(usedChannels);
    if (errorCode != PAC193X_NO_ERROR) {
        return PAC193X_INIT_ERROR;
    }

    /* sample rate of 1024Hz, enable continuous measurement mode, enable overflow alert */
    errorCode = sendConfigurationToSensor(PAC193X_CMD_CTRL, 0b00001010);
    if (errorCode != PAC193X_NO_ERROR) {
        return PAC193X_INIT_ERROR;
    }

    /* sets measurement to unipolar mode */
    errorCode = sendConfigurationToSensor(PAC193X_CMD_NEG_PWR, 0b00000000);
    if (errorCode != PAC193X_NO_ERROR) {
        return PAC193X_INIT_ERROR;
    }

    /* enable limited refresh function and set POR to 0 */
    errorCode = sendConfigurationToSensor(PAC193X_CMD_SLOW, 0b00010100);
    if (errorCode != PAC193X_NO_ERROR) {
        return PAC193X_INIT_ERROR;
    }

    /* refresh sensor to use updated settings */
    errorCode = refresh();
    if (errorCode != PAC193X_NO_ERROR) {
        return PAC193X_INIT_ERROR;
    }

    return PAC193X_NO_ERROR;
}

pac193x_errorCode pac193x_setResistanceAtSense(const float resistanceValues[4]) {
    sensorConfiguration.rSense[0] = resistanceValues[0];
    sensorConfiguration.rSense[1] = resistanceValues[1];
    sensorConfiguration.rSense[2] = resistanceValues[2];
    sensorConfiguration.rSense[3] = resistanceValues[3];

    return PAC193X_NO_ERROR;
}

pac193x_errorCode pac193x_setChannelsInUse(pac193x_usedChannels usedChannels) {
    /* check if used channels are valid */
    if (usedChannels.uint_channelsInUse > 0b00010000) {
        return PAC193X_INIT_ERROR;
    }

    /* update configuration */
    sensorConfiguration.usedChannels = usedChannels;

    /* send configuration to sensor */
    uint8_t channelsInUse = (usedChannels.uint_channelsInUse << 4) ^ 0xF0;
    pac193x_errorCode errorCode = sendConfigurationToSensor(PAC193X_CMD_CHANNEL_DIS, channelsInUse);
    if (errorCode != PAC193X_NO_ERROR) {
        return PAC193X_INIT_ERROR;
    }

    return PAC193X_NO_ERROR;
}

pac193x_errorCode pac193x_getSensorInfo(pac193x_info *info) {
    uint8_t sizeOfResponseBuffer = 1;

    pac193x_errorCode errorCode = getDataFromSensor(&info->product_id, sizeOfResponseBuffer,
                                                    PAC193X_CMD_READ_PRODUCT_ID);
    if (errorCode != PAC193X_NO_ERROR) {
        info->product_id = 0;
        info->manufacturer_id = 0;
        info->revision_id = 0;
        return errorCode;
    }

    errorCode = getDataFromSensor(&info->manufacturer_id, sizeOfResponseBuffer, PAC193X_CMD_READ_MANUFACTURER_ID);
    if (errorCode != PAC193X_NO_ERROR) {
        info->product_id = 0;
        info->manufacturer_id = 0;
        info->revision_id = 0;
        return errorCode;
    }

    errorCode = getDataFromSensor(&info->revision_id, sizeOfResponseBuffer, PAC193X_CMD_READ_REVISION_ID);
    if (errorCode != PAC193X_NO_ERROR) {
        info->product_id = 0;
        info->manufacturer_id = 0;
        info->revision_id = 0;
        return errorCode;
    }

    return PAC193X_NO_ERROR;
}

pac193x_errorCode
pac193x_getMeasurementForChannel(pac193x_channel channel, pac193x_valueToMeasure valueToMeasure, float *value) {
    /* check if channel is activated */
    bool validChannel;
    switch (channel) {
        case (PAC193X_CHANNEL01):
            validChannel = sensorConfiguration.usedChannels.struct_channelsInUse.channel1 == 1;
            break;
        case (PAC193X_CHANNEL02):
            validChannel = sensorConfiguration.usedChannels.struct_channelsInUse.channel2 == 1;
            break;
        case (PAC193X_CHANNEL03):
            validChannel = sensorConfiguration.usedChannels.struct_channelsInUse.channel3 == 1;
            break;
        case (PAC193X_CHANNEL04):
            validChannel = sensorConfiguration.usedChannels.struct_channelsInUse.channel4 == 1;
            break;
        default:
            validChannel = false;
    }
    if (!validChannel) {
        return PAC193X_INVALID_CHANNEL;
    }

    /* store configurations for measurements */
    pac193x_measurementProperties properties;
    pac193x_errorCode errorCode = setMeasurementProperties(&properties, valueToMeasure);
    if (errorCode != PAC193X_NO_ERROR) {
        return errorCode;
    }

    /* retrieve data from sensor */
    uint8_t responseBuffer[properties.sizeOfResponseBuffer];
    errorCode = getDataFromSensor(responseBuffer, properties.sizeOfResponseBuffer, properties.startReadAddress);
    if (errorCode != PAC193X_NO_ERROR) {
        return errorCode;
    }

    /* transform raw data */
    uint64_t rawValue = transformResponseBufferToUInt64(responseBuffer, properties.sizeOfResponseBuffer);
    *value = (*properties.calculationFunction)(rawValue, translateChannelToRSenseArrayIndex(channel));

    return errorCode;
}

pac193x_errorCode pac193x_getAllMeasurementsForChannel(pac193x_channel channel, pac193x_measurements *measurements) {
    /* refresh to get latest values */
    refreshV();

    pac193x_errorCode errorCode = pac193x_getMeasurementForChannel(channel, PAC193X_VSOURCE,
                                                                   &measurements->voltageSource);
    if (errorCode != PAC193X_NO_ERROR) {
        return errorCode;
    }

    errorCode = pac193x_getMeasurementForChannel(channel, PAC193X_VSENSE, &measurements->voltageSense);
    if (errorCode != PAC193X_NO_ERROR) {
        return errorCode;
    }

    errorCode = pac193x_getMeasurementForChannel(channel, PAC193X_ISENSE, &measurements->iSense);
    if (errorCode != PAC193X_NO_ERROR) {
        return errorCode;
    }

    errorCode = pac193x_getMeasurementForChannel(channel, PAC193X_PACTUAL, &measurements->powerActual);
    if (errorCode != PAC193X_NO_ERROR) {
        return errorCode;
    }

    errorCode = pac193x_getMeasurementForChannel(channel, PAC193X_ENERGY, &measurements->energy);

    return errorCode;
}

/* endregion */

/* region STATIC FUNCTION IMPLEMENTATIONS */

static pac193x_errorCode refresh(void) {
    PRINT_DEBUG("send refresh signal to sensor")
    /* trigger refresh */
    pac193x_errorCode errorCode = sendRequestToSensor(PAC193X_CMD_REFRESH);
    if (errorCode != PAC193X_NO_ERROR) {
        PRINT_DEBUG("refresh send failed, error was %02X", errorCode)
        return errorCode;
    }

    /* sleep because sensor is unreachable for 1ms after refresh */
    sleep_ms(1);

    PRINT_DEBUG("refresh successful")
    return PAC193X_NO_ERROR;
}

static pac193x_errorCode refreshV(void) {
    PRINT_DEBUG("send refreshV signal to sensor")
    /* trigger refresh */
    pac193x_errorCode errorCode = sendRequestToSensor(PAC193X_CMD_REFRESH_V);
    if (errorCode != PAC193X_NO_ERROR) {
        PRINT_DEBUG("refreshV send failed, error was %02X", errorCode)
        return errorCode;
    }

    /* sleep because sensor is unreachable for 1ms after refresh */
    sleep_ms(1);

    PRINT_DEBUG("refreshV successful")
    return PAC193X_NO_ERROR;
}


static uint8_t translateChannelToRSenseArrayIndex(pac193x_channel channel) {
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

static pac193x_errorCode
setMeasurementProperties(pac193x_measurementProperties *properties, pac193x_valueToMeasure valueToMeasure) {
    PRINT_DEBUG("setting properties for requested measurement")
    /*! address of a value to be measured are separated by 0x01 per channel:
     *   -> CMD_READ_VBUS1 = 0x07, CMD_READ_VBUS2 = 0x08, ...
     * \Important actual address will be added later by setMeasurementProperties(...) function
     */
    properties->startReadAddress = 1;

    switch (valueToMeasure) {
        case PAC193X_VSOURCE:
            properties->startReadAddress += PAC193X_CMD_READ_VBUS1;
            properties->calculationFunction = &calculateVoltageOfSource;
            properties->sizeOfResponseBuffer = 2;
            break;
        case PAC193X_VSOURCE_AVG:
            properties->startReadAddress += PAC193X_CMD_READ_VBUS1_AVG;
            properties->calculationFunction = &calculateVoltageOfSource;
            properties->sizeOfResponseBuffer = 2;
            break;
        case PAC193X_VSENSE:
            properties->startReadAddress += PAC193X_CMD_READ_VSENSE1;
            properties->calculationFunction = &calculateVoltageOfSense;
            properties->sizeOfResponseBuffer = 2;
            break;
        case PAC193X_VSENSE_AVG:
            properties->startReadAddress += PAC193X_CMD_READ_VSENSE1_AVG;
            properties->calculationFunction = &calculateVoltageOfSense;
            properties->sizeOfResponseBuffer = 2;
            break;
        case PAC193X_ISENSE:
            properties->startReadAddress += PAC193X_CMD_READ_VSENSE1;
            properties->calculationFunction = &calculateCurrentOfSense;
            properties->sizeOfResponseBuffer = 2;
            break;
        case PAC193X_ISENSE_AVG:
            properties->startReadAddress += PAC193X_CMD_READ_VSENSE1_AVG;
            properties->calculationFunction = &calculateCurrentOfSense;
            properties->sizeOfResponseBuffer = 2;
            break;
        case PAC193X_PACTUAL:
            properties->startReadAddress += PAC193X_CMD_READ_VPOWER1;
            properties->calculationFunction = &calculateActualPower;
            properties->sizeOfResponseBuffer = 4;
            break;
        case PAC193X_ENERGY:
            properties->startReadAddress += PAC193X_CMD_READ_VPOWER1_ACC;
            properties->calculationFunction = &calculateEnergy;
            properties->sizeOfResponseBuffer = 6;
            break;
        default: PRINT_DEBUG("invalid properties")
            return PAC193X_INVALID_MEASUREMENT;
    }

    PRINT_DEBUG("settings applied successful")
    return PAC193X_NO_ERROR;
}

static pac193x_errorCode
sendConfigurationToSensor(pac193x_registerAddress registerToWrite, pac193x_settings settingsToWrite) {
    uint8_t sizeOfCommandBuffer = 2;
    uint8_t commandBuffer[sizeOfCommandBuffer];
    commandBuffer[0] = registerToWrite;
    commandBuffer[1] = settingsToWrite;

    PRINT_DEBUG("send configuration to sensor")
    /* send new configuration to sensor */
    I2C_ErrorCode i2cErrorCode = I2C_WriteCommand(commandBuffer, sizeOfCommandBuffer,
                                                  sensorConfiguration.i2c_slave_address, sensorConfiguration.i2c_host
    );
    if (i2cErrorCode != I2C_NO_ERROR) {
        PRINT_DEBUG("send configuration failed, error was %02X", i2cErrorCode)
        return PAC193X_SEND_COMMAND_ERROR;
    }
    PRINT_DEBUG("configuration send successful")

    pac193x_errorCode errorCode = refreshV();
    if (errorCode != PAC193X_NO_ERROR) {
        return errorCode;
    }

    return PAC193X_NO_ERROR;
}

static pac193x_errorCode sendRequestToSensor(pac193x_registerAddress registerToRead) {
    uint8_t sizeOfCommandBuffer = 1;
    uint8_t commandBuffer[sizeOfCommandBuffer];
    commandBuffer[0] = registerToRead;

    PRINT_DEBUG("request data from sensor")
    I2C_ErrorCode errorCode = I2C_WriteCommand(commandBuffer, sizeOfCommandBuffer,
                                               sensorConfiguration.i2c_slave_address, sensorConfiguration.i2c_host);
    if (errorCode != I2C_NO_ERROR) {
        PRINT_DEBUG("sending request failed, error was %02X", errorCode)
        return PAC193X_SEND_COMMAND_ERROR;
    }

    return PAC193X_NO_ERROR;
}

static pac193x_errorCode receiveDataFromSensor(uint8_t *responseBuffer, uint8_t sizeOfResponseBuffer) {
    PRINT_DEBUG("receiving data from sensor")
    I2C_ErrorCode errorCode = I2C_ReadData(responseBuffer, sizeOfResponseBuffer, sensorConfiguration.i2c_slave_address,
                                           sensorConfiguration.i2c_host);
    if (errorCode != I2C_NO_ERROR) {
        PRINT_DEBUG("receiving data failed, error was %02X", errorCode)
        return PAC193X_RECEIVE_DATA_ERROR;
    }
    PRINT_DEBUG("received data successful")

    return PAC193X_NO_ERROR;
}

static pac193x_errorCode
getDataFromSensor(uint8_t *responseBuffer, uint8_t sizeOfResponseBuffer, pac193x_registerAddress registerToRead) {
    /* trigger refresh to store current values for request */
    pac193x_errorCode errorCode = refresh();
    if (errorCode != PAC193X_NO_ERROR) {
        return errorCode;
    }

    errorCode = sendRequestToSensor(registerToRead);
    if (errorCode != PAC193X_NO_ERROR) {
        return errorCode;
    }

    errorCode = receiveDataFromSensor(responseBuffer, sizeOfResponseBuffer);
    if (errorCode != PAC193X_NO_ERROR) {
        return errorCode;
    }

    return PAC193X_NO_ERROR;
}


static uint64_t transformResponseBufferToUInt64(const uint8_t *responseBuffer, uint8_t sizeOfResponseBuffer) {
    PRINT_DEBUG("transforming buffer to uint64")
    uint64_t scalar = responseBuffer[0];
    for (uint8_t index = 1; index < sizeOfResponseBuffer; index++) {
        scalar <<= 8;
        scalar |= (uint64_t) (responseBuffer[index]);
    }

    PRINT_DEBUG("output: %lu", scalar)
    return scalar;
}

static float convertToFloat(uint64_t input) {
    PRINT_DEBUG("converting value to float")
    PRINT_DEBUG("output: %f", ((float) input))
    return (float) input;
}

static float calculateVoltageOfSource(uint64_t input, uint8_t channel) {
    PRINT_DEBUG("calculating voltage of source")
    float vSource = 32.0f * (convertToFloat(input) / UNIPOLAR_VOLTAGE_DENOMINATOR);
    PRINT_DEBUG("output: %f", vSource)
    return vSource;
}

static float calculateVoltageOfSense(uint64_t input, uint8_t channel) {
    PRINT_DEBUG("calculating voltage of sense")
    float vSense = 0.1f * (convertToFloat(input) / UNIPOLAR_VOLTAGE_DENOMINATOR);
    PRINT_DEBUG("output: %f", vSense)
    return vSense;
}

static float calculateCurrentOfSense(uint64_t input, uint8_t channel) {
    PRINT_DEBUG("calculating current of sense")
    float fsc = 0.1f / sensorConfiguration.rSense[channel];
    float iSense = fsc * (convertToFloat(input) / UNIPOLAR_VOLTAGE_DENOMINATOR);
    PRINT_DEBUG("output: %f", iSense)
    return iSense;
}

static float calculateActualPower(uint64_t input, uint8_t channel) {
    PRINT_DEBUG("calculating actual power")
    float powerFSR = 3.2f / sensorConfiguration.rSense[channel];
    float powerConversionFactor = convertToFloat(input) / UNIPOLAR_POWER_DENOMINATOR;
    float powerActual = powerFSR * powerConversionFactor;
    PRINT_DEBUG("output: %f", powerActual)
    return powerActual;
}

static float calculateEnergy(uint64_t input, uint8_t channel) {
    PRINT_DEBUG("calculating energy")
    float powerFSR = 3.2f / sensorConfiguration.rSense[channel];
    float energy = convertToFloat(input) * powerFSR / (ENERGY_DENOMINATOR * SAMPLING_RATE);
    PRINT_DEBUG("output: %f", energy)
    return energy;
}

/* endregion */

