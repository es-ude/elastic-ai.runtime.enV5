//
// Created by David P. Federl
//

#include "pac193x.h"
#include "gpio.h"
#include "i2c.h"
#include <pico/time.h>

/*******************************************************/
/* region DATATYPE / VARIABLES */

/* Denominator for unipolar voltage measurement: 2^16 = 65536 */
static const float VOLTAGE_DENOMINATOR = (float)(1U << 16);

/* Denominator for unipolar power measurement: 2^32 = 4294967296
 * Note that this denominator is 2^28 according to the datasheet,
 * however testing has shown that 2^32 is correct.
 */
static const float POWER_DENOMINATOR  = (float)(1ULL << 32);

static const float ENERGY_DENOMINATOR = (float)(1ULL << 28);

/* sampling rate can be set using the ctrl register */
static const float SAMPLING_RATE = 1024;

static pac193x_sensor_configuration sensorConfiguration;

/* endregion */
/*******************************************************/
/* region STATIC FUNCTION PROTOTYPES */

static pac193x_errorCode checkSensorOnBus();

static pac193x_errorCode refresh(void);
static pac193x_errorCode refreshV(void);
static pac193x_errorCode refreshG(void);

static pac193x_errorCode setMeasurementProperties(pac193x_measurement_properties *properties,
                                                  pac193x_valueToMeasure          valueToMeasure);

static pac193x_errorCode sendConfigurationToSensor(pac193x_registerAddress registerToWrite,
                                                   pac193x_settings        settingsToWrite);
static pac193x_errorCode sendRequestToSensor(pac193x_registerAddress registerToRead);
static pac193x_errorCode receiveDataFromSensor(uint8_t *responseBuffer, uint8_t sizeOfResponseBuffer);
static pac193x_errorCode getDataFromSensor(uint8_t *responseBuffer, uint8_t sizeOfResponseBuffer,
                                           pac193x_registerAddress registerToRead);

static uint64_t transformResponseBufferToUInt64(const uint8_t *responseBuffer, uint8_t sizeOfResponseBuffer);
static float    convertToFloat(uint64_t input);
static float    calculateVoltageSense(uint64_t voltageSense, uint8_t channel);
static float    calculateVoltageSource(uint64_t voltageBus, uint8_t channel);
static float    calculateISense(uint64_t valueSense, uint8_t channel);
static float    calculatePowerActual(uint64_t valuePower, uint8_t channel);
static float    calculateEnergy(uint64_t valueAccumulator, uint8_t channel);

/* endregion */
/*******************************************************/
/* region HEADER FUNCTION IMPLEMENTATIONS */

pac193x_errorCode pac193x_init(i2c_inst_t *i2cHost, float resistanceValues[4],
                               uint8_t numberOfChannelsInUse) {
    /* save i2c access for later usage */
    sensorConfiguration.i2c_host          = i2cHost;
    sensorConfiguration.i2c_slave_address = 0x11;
    
    pac193x_setResistanceAtSense(resistanceValues);
    
    GPIO_setPower(29, GPIO_POWER_UP);
    
    /* sensor needs max 10ms to power up after power enabled */
    sleep_ms(10);
    
    I2C_Init(sensorConfiguration.i2c_host, (100 * 1000), 6, 7);
    
    pac193x_errorCode errorCode = checkSensorOnBus(sensorConfiguration.i2c_slave_address);
    if (errorCode != PAC193X_NO_ERROR) {
        return PAC193X_INIT_ERROR;
      }
    
    errorCode = pac193x_setNumberOfChannelsInUse(numberOfChannelsInUse);
    if (errorCode != PAC193X_NO_ERROR) {
        return PAC193X_INIT_ERROR;
      }
    
    /* enable overflow alert */
    errorCode = sendConfigurationToSensor(PAC193X_CMD_CTRL, 0b00000010);
    if (errorCode != PAC193X_NO_ERROR) {
        return PAC193X_INIT_ERROR;
      }
    
    /* default settings */
    errorCode = sendConfigurationToSensor(PAC193X_CMD_NEG_PWR, 0b00000000);
    if (errorCode != PAC193X_NO_ERROR) {
        return PAC193X_INIT_ERROR;
      }
    
    /* enable limited refresh function */
    errorCode = sendConfigurationToSensor(PAC193X_CMD_SLOW, 0b00010100);
    if (errorCode != PAC193X_NO_ERROR) {
        return PAC193X_INIT_ERROR;
      }
    
    /* refresh sensor to use updated settings */
    errorCode = refresh();
    return errorCode;
  }

void pac193x_setResistanceAtSense(float resistanceValues[4]) {
    for (int i = 0; i < 4; i++) {
        sensorConfiguration.rSense[i] = resistanceValues[i];
      }
  }

pac193x_errorCode pac193x_setNumberOfChannelsInUse(uint8_t numberOfChannelsInUse) {
    if (numberOfChannelsInUse < 1 || numberOfChannelsInUse > 4) {
        return PAC193X_INIT_ERROR;
      }
    
    sensorConfiguration.highestChannelInUse = numberOfChannelsInUse;
    
    uint8_t bitmask                         = 0xF0;
    uint8_t disabledChannels                = 0xFF >> numberOfChannelsInUse;
    disabledChannels &= bitmask;
    
    pac193x_errorCode errorCode = sendConfigurationToSensor(PAC193X_CMD_CHANNEL_DIS, disabledChannels);
    if (errorCode != PAC193X_NO_ERROR) {
        return PAC193X_INIT_ERROR;
      }
    
    return errorCode;
  }

pac193x_errorCode pac193x_getSensorInfo(pac193x_info *info) {
    pac193x_errorCode errorCode;
    uint8_t           sizeOfResponseBuffer = 1;
    
    errorCode = getDataFromSensor(&info->product_id, sizeOfResponseBuffer, PAC193X_CMD_READ_PRODUCT_ID);
    if (errorCode != PAC193X_NO_ERROR) {
        return errorCode;
      }
    
    errorCode =
            getDataFromSensor(&info->manufacturer_id, sizeOfResponseBuffer, PAC193X_CMD_READ_MANUFACTURER_ID);
    if (errorCode != PAC193X_NO_ERROR) {
        return errorCode;
      }
    
    errorCode = getDataFromSensor(&info->revision_id, sizeOfResponseBuffer, PAC193X_CMD_READ_REVISION_ID);
    return errorCode;
  }

pac193x_errorCode pac193x_getMeasurementForChannel(pac193x_channel        channel,
                                                   pac193x_valueToMeasure valueToMeasure, float *value) {
    if (channel < 1 || channel > sensorConfiguration.highestChannelInUse) {
        return PAC193X_INVALID_CHANNEL;
      }
    
    pac193x_measurement_properties properties;
    
    /* command address of a measurement type are separated by 1:
     *   -> CMD_READ_VBUS1 = 0x07, CMD_READ_VBUS2 = 0x08, ...
     * IMPORTANT: actual command will be added later by SetMeasurementProperties(...) function
     */
    properties.command          = (channel - 1);
    
    pac193x_errorCode errorCode = setMeasurementProperties(&properties, valueToMeasure);
    if (errorCode != PAC193X_NO_ERROR) {
        return errorCode;
      }
    uint8_t responseBuffer[properties.sizeOfResponseBuffer];
    
    errorCode = getDataFromSensor(responseBuffer, properties.sizeOfResponseBuffer, properties.command);
    if (errorCode != PAC193X_NO_ERROR) {
        return errorCode;
      }
    
    uint64_t rawValue = transformResponseBufferToUInt64(responseBuffer, properties.sizeOfResponseBuffer);
    
    *value            = (*properties.calculationFunction)(rawValue, channel);
    
    return errorCode;
  }

pac193x_errorCode pac193x_getEnergyAndResetAccumulation(float energy[4]) {
    pac193x_errorCode errorCode = refresh();
    if (errorCode != PAC193X_NO_ERROR) {
        return errorCode;
      }
    
    for (int i = 0; i < sensorConfiguration.highestChannelInUse; i++) {
        errorCode = pac193x_getMeasurementForChannel(i + 1, PAC193X_ENERGY, &energy[i]);
        if (errorCode != PAC193X_NO_ERROR) {
            return errorCode;
          }
      }
    return errorCode;
  }

pac193x_errorCode pac193x_getAllMeasurementsForChannel(pac193x_channel       channel,
                                                       pac193x_measurements *measurements) {
    /* refresh to get latest values */
    refreshV();
    
    pac193x_errorCode errorCode =
                              pac193x_getMeasurementForChannel(channel, PAC193X_VSOURCE, &measurements->voltageSource);
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
/*******************************************************/
/* region STATIC FUNCTION IMPLEMENTATIONS */

static pac193x_errorCode checkSensorOnBus() {
    uint8_t responseBuffer[2];
    return receiveDataFromSensor(responseBuffer, sizeof(responseBuffer));
  }

static pac193x_errorCode refresh(void) {
    pac193x_errorCode errorCode = sendRequestToSensor(PAC193X_CMD_REFRESH);
    sleep_ms(1);
    return errorCode;
  }
static pac193x_errorCode refreshV(void) {
    pac193x_errorCode errorCode = sendRequestToSensor(PAC193X_CMD_REFRESH_V);
    sleep_ms(1);
    return errorCode;
  }
static pac193x_errorCode refreshG(void) {
    pac193x_errorCode errorCode = sendRequestToSensor(PAC193X_CMD_REFRESH_G);
    sleep_ms(1);
    return errorCode;
  }

static pac193x_errorCode setMeasurementProperties(pac193x_measurement_properties *properties,
                                                  pac193x_valueToMeasure          valueToMeasure) {
    switch (valueToMeasure) {
        case PAC193X_VSOURCE:
          properties->command += PAC193X_CMD_READ_VBUS1;
        properties->calculationFunction  = &calculateVoltageSource;
        properties->sizeOfResponseBuffer = 2;
        
        break;
        case PAC193X_VSOURCE_AVG:
          properties->command += PAC193X_CMD_READ_VBUS1_AVG;
        properties->calculationFunction  = &calculateVoltageSource;
        properties->sizeOfResponseBuffer = 2;
        
        break;
        case PAC193X_VSENSE:
          properties->command += PAC193X_CMD_READ_VSENSE1;
        properties->calculationFunction  = &calculateVoltageSense;
        properties->sizeOfResponseBuffer = 2;
        
        break;
        case PAC193X_VSENSE_AVG:
          properties->command += PAC193X_CMD_READ_VSENSE1_AVG;
        properties->calculationFunction  = &calculateVoltageSense;
        properties->sizeOfResponseBuffer = 2;
        
        break;
        case PAC193X_ISENSE:
          properties->command += PAC193X_CMD_READ_VSENSE1;
        properties->calculationFunction  = &calculateISense;
        properties->sizeOfResponseBuffer = 2;
        
        break;
        case PAC193X_ISENSE_AVG:
          properties->command += PAC193X_CMD_READ_VSENSE1_AVG;
        properties->calculationFunction  = &calculateISense;
        properties->sizeOfResponseBuffer = 2;
        
        break;
        case PAC193X_PACTUAL:
          properties->command += PAC193X_CMD_READ_VPOWER1;
        properties->calculationFunction  = &calculatePowerActual;
        properties->sizeOfResponseBuffer = 4;
        break;
        case PAC193X_ENERGY:
          properties->command += PAC193X_CMD_READ_VPOWER1_ACC;
        properties->calculationFunction  = &calculateEnergy;
        properties->sizeOfResponseBuffer = 6;
        break;
        default:
          return PAC193X_INVALID_MEASUREMENT;
      }
    
    return PAC193X_NO_ERROR;
  }

static pac193x_errorCode sendConfigurationToSensor(pac193x_registerAddress registerToWrite,
                                                   pac193x_settings        settingsToWrite) {
    uint8_t sizeOfCommandBuffer = 2;
    uint8_t commandBuffer[sizeOfCommandBuffer];
    commandBuffer[0] = registerToWrite;
    commandBuffer[1] = settingsToWrite;
    
    I2C_ErrorCode errorCode =
                          I2C_WriteCommand(commandBuffer, sizeOfCommandBuffer, sensorConfiguration.i2c_slave_address,
                                           sensorConfiguration.i2c_host);
    
    if (errorCode != I2C_NO_ERROR) {
        return PAC193X_SEND_COMMAND_ERROR;
      }
    return PAC193X_NO_ERROR;
  }
static pac193x_errorCode sendRequestToSensor(pac193x_registerAddress registerToRead) {
    uint8_t sizeOfCommandBuffer = 1;
    uint8_t commandBuffer[sizeOfCommandBuffer];
    commandBuffer[0] = registerToRead;
    
    I2C_ErrorCode errorCode =
                          I2C_WriteCommand(commandBuffer, sizeOfCommandBuffer, sensorConfiguration.i2c_slave_address,
                                           sensorConfiguration.i2c_host);
    
    if (errorCode != I2C_NO_ERROR) {
        return PAC193X_SEND_COMMAND_ERROR;
      }
    return PAC193X_NO_ERROR;
  }
static pac193x_errorCode receiveDataFromSensor(uint8_t *responseBuffer, uint8_t sizeOfResponseBuffer) {
    I2C_ErrorCode errorCode =
                          I2C_ReadData(responseBuffer, sizeOfResponseBuffer, sensorConfiguration.i2c_slave_address,
                                       sensorConfiguration.i2c_host);
    
    if (errorCode != I2C_NO_ERROR) {
        return PAC193X_RECEIVE_DATA_ERROR;
      }
    return PAC193X_NO_ERROR;
  }
static pac193x_errorCode getDataFromSensor(uint8_t *responseBuffer, uint8_t sizeOfResponseBuffer,
                                           pac193x_registerAddress registerToRead) {
    pac193x_errorCode errorCode = sendRequestToSensor(registerToRead);
    if (errorCode != PAC193X_NO_ERROR) {
        return errorCode;
      }
    
    errorCode = receiveDataFromSensor(responseBuffer, sizeOfResponseBuffer);
    return errorCode;
  }

static uint64_t transformResponseBufferToUInt64(const uint8_t *responseBuffer, uint8_t sizeOfResponseBuffer) {
    uint64_t scalar = responseBuffer[0];
    for (uint8_t i = 1; i < sizeOfResponseBuffer; i++) {
        scalar <<= 8;
        scalar |= (uint64_t)(responseBuffer[i]);
      }
    return scalar;
  }
static float convertToFloat(uint64_t input) { return (float)input; }

static float calculateVoltageSource(uint64_t voltageBus, uint8_t channel) {
    float vSource = 32.0f * (convertToFloat(voltageBus) / VOLTAGE_DENOMINATOR);
    return vSource;
  }
static float calculateVoltageSense(uint64_t voltageSense, uint8_t channel) {
    float vSense = 0.1f * (convertToFloat(voltageSense) / VOLTAGE_DENOMINATOR);
    return vSense;
  }
static float calculateISense(uint64_t valueSense, uint8_t channel) {
    float fsc    = 0.1f / sensorConfiguration.rSense[channel - 1];
    float iSense = fsc * (convertToFloat(valueSense) / VOLTAGE_DENOMINATOR);
    return iSense;
  }
static float calculatePowerActual(uint64_t valuePower, uint8_t channel) {
    float powerFSR              = 3.2f / sensorConfiguration.rSense[channel - 1];
    float powerConversionFactor = convertToFloat(valuePower) / POWER_DENOMINATOR;
    float powerActual           = powerFSR * powerConversionFactor;
    return powerActual;
  }
static float calculateEnergy(uint64_t valueAccumulator, uint8_t channel) {
    float powerFSR = 3.2f / sensorConfiguration.rSense[channel - 1];
    float energy   = convertToFloat(valueAccumulator) * powerFSR / (ENERGY_DENOMINATOR * SAMPLING_RATE);
    return energy;
  }

/* endregion */

