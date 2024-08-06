#define SOURCE_FILE "BMI323-SPI-ADAPTER"

/*!
 * Datasheet
 *    https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bmi323-ds000.pdf
 */

#include <math.h>

#include "CException.h"

#include "Bmi323.h"
#include "Sleep.h"
#include "Spi.h"
#include "SpiTypedefs.h"

#include "bmi3.h"
#include "bmi323.h"
#include "bmi323_defs.h"
#include "bmi3_defs.h"

/* region INTERNAL FUNCTIONS */

static void bmi323ProcessErrorCode(int8_t errorCode) {
    if (errorCode == BMI323_OK) {
        return;
    }

    Throw(errorCode);
}

static BMI3_INTF_RET_TYPE bmi3_spi_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len,
                                        void *intf_ptr) {
    data_t command = {.data = &reg_addr, .length = 1};
    data_t result = {.data = reg_data, .length = len};

    int readBytes = spiWriteCommandAndReadBlocking(intf_ptr, &command, &result);

    return readBytes == len ? BMI3_OK : BMI3_E_COM_FAIL;
}
static BMI3_INTF_RET_TYPE bmi3_spi_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len,
                                         void *intf_ptr) {
    data_t command = {.data = &reg_addr, .length = 1};
    data_t data = {.data = reg_data, .length = len};

    int readBytes = spiWriteCommandAndDataBlocking(intf_ptr, &command, &data);

    return readBytes == len ? BMI3_OK : BMI3_E_COM_FAIL;
}

static void bmi3_delay_us(uint32_t period, void *intf_ptr) {
    sleep_for_us(period);
}

static float getMaxDpsForRange(uint8_t range) {
    switch (range) {
    case BMI3_GYR_RANGE_125DPS:
        return 125.0f;
    case BMI3_GYR_RANGE_250DPS:
        return 250.0f;
    case BMI3_GYR_RANGE_500DPS:
        return 500.0f;
    case BMI3_GYR_RANGE_1000DPS:
        return 1000.0f;
    case BMI3_GYR_RANGE_2000DPS:
        return 2000.0f;
    default:
        return 0;
    }
}

/* endregion INTERNAL FUNCTIONS */

/* region PUBLIC FUNCTIONS */

/* region CONFIGRATION */

void bmi323Init(bmi323SensorConfiguration_t *sensor, spiConfiguration_t *spi) {
    if (sensor != NULL) {
        sensor->intf = BMI3_SPI_INTF;
        sensor->read = bmi3_spi_read;
        sensor->write = bmi3_spi_write;
        sensor->delay_us = bmi3_delay_us;
        sensor->intf_ptr = spi;
        sensor->read_write_len = 8;

        bmi323ProcessErrorCode(bmi3_init(sensor));
    } else {
        bmi323ProcessErrorCode(BMI3_E_NULL_PTR);
    }
}

void bmi323GetRegister(bmi323SensorConfiguration_t *sensor, uint8_t address, data_t *data) {
    bmi323ProcessErrorCode(bmi323_get_regs(address, data->data, data->length, sensor));
}
void bmi323SetRegister(bmi323SensorConfiguration_t *sensor, uint8_t address, data_t *data) {
    bmi323ProcessErrorCode(bmi323_set_regs(address, data->data, data->length, sensor));
}

void bmi323EnableFeature(bmi323SensorConfiguration_t *sensor, bmi323Features *features) {
    bmi323ProcessErrorCode(bmi323_select_sensor(features, sensor));
}
void bmi323SetSensorConfiguration(bmi323SensorConfiguration_t *sensor, uint8_t feature,
                                  bmi323FeatureConfiguration *config) {
    bmi323ProcessErrorCode(bmi323_set_sensor_config(config, feature, sensor));
}
void bmi323GetSensorConfiguration(bmi323SensorConfiguration_t *sensor, uint8_t feature,
                                  bmi323FeatureConfiguration *config) {
    bmi323ProcessErrorCode(bmi323_get_sensor_config(config, feature, sensor));
}

void bmi323GetInterruptConfig(bmi323SensorConfiguration_t *sensor,
                              bmi323InterruptConfig_t *config) {
    bmi323ProcessErrorCode(bmi323_get_int_pin_config(config, sensor));
}
void bmi323SetInterruptConfig(bmi323SensorConfiguration_t *sensor,
                              bmi323InterruptConfig_t *config) {
    bmi323ProcessErrorCode(bmi323_set_int_pin_config(config, sensor));
}
void bmi323MapInterrupt(bmi323SensorConfiguration_t *sensor, bmi323InterruptMapping mapping) {
    bmi323ProcessErrorCode(bmi323_map_interrupt(mapping, sensor));
}
uint16_t bmi323GetInterruptStatus(bmi323SensorConfiguration_t *sensor,
                                  bmi323InterruptRegister_t interruptRegister) {
    uint16_t interruptState = 0;
    if (interruptRegister == BMI323_INTERRUPT_1) {
        bmi323ProcessErrorCode(bmi323_get_int1_status(&interruptState, sensor));
    } else if (interruptRegister == BMI323_INTERRUPT_2) {
        bmi323ProcessErrorCode(bmi323_get_int2_status(&interruptState, sensor));
    } else {
        bmi323ProcessErrorCode(BMI3_E_INVALID_INT_PIN);
    }

    return interruptState;
}

void bmi323GetRemappingOfAxes(bmi323SensorConfiguration_t *sensor, bmi323AxesRemap *remapping) {
    bmi323ProcessErrorCode(bmi3_get_remap_axes(remapping, sensor));
}
void bmi323SetRemappingOfAxes(bmi323SensorConfiguration_t *sensor, bmi323AxesRemap remapping) {
    bmi323ProcessErrorCode(bmi3_set_remap_axes(remapping, sensor));
}

/* endregion CONFIGURATION */

/* region DATA */

void bmi323GetData(bmi323SensorConfiguration_t *sensor, uint8_t feature, bmi323SensorData_t *data) {
    bmi323ProcessErrorCode(bmi323_get_sensor_data(data, feature, sensor));
}

float bmi323GetSensorTime(bmi323SensorConfiguration_t *sensor) {
    uint32_t time = 0;
    bmi323ProcessErrorCode(bmi323_get_sensor_time(&time, sensor));
    return (float)time * BMI3_SENSORTIME_RESOLUTION;
}

uint16_t bmi323GetTemperature(bmi323SensorConfiguration_t *sensor) {
    uint16_t temperature;
    bmi323ProcessErrorCode(bmi323_get_temperature_data(&temperature, sensor));
    return temperature;
}

float bmi323LsbToDps(int16_t rawValue, uint8_t range, uint8_t resolution) {
    float halfScale = powf(2.0f, resolution) / 2.0f;
    float dpsRange = getMaxDpsForRange(range);
    return (dpsRange / halfScale) * ((float)rawValue);
}

/* endregion DATA */

/* region FIFO */

void bmi323SetFifoConfiguration(bmi323SensorConfiguration_t *sensor, bool enable,
                                uint16_t configuration) {
    bmi323ProcessErrorCode(bmi323_set_fifo_config(configuration, enable, sensor));
}

uint16_t bmi323GetFifoConfiguration(bmi323SensorConfiguration_t *sensor) {
    uint16_t config = 0;
    bmi323ProcessErrorCode(bmi323_get_fifo_config(&config, sensor));
    return config;
}

uint16_t bmi323GetFifoLength(bmi323SensorConfiguration_t *sensor) {
    uint16_t length = 0;
    bmi323ProcessErrorCode(bmi323_get_fifo_length(&length, sensor));
    return length;
}

void bmi323GetFifoFrame(bmi323SensorConfiguration_t *sensor, bmi323FifoFrame_t *frame) {
    bmi323ProcessErrorCode(bmi323_read_fifo_data(frame, sensor));
}
void bmi323ExtractAcceleratorData(bmi323SensorConfiguration_t *sensor, bmi323FifoFrame_t *frame,
                                  bmi323AxesData_t *data) {
    bmi323ProcessErrorCode(bmi323_extract_accel(data, frame, sensor));
}

void bmi323ExtractGyroscopeData(bmi323SensorConfiguration_t *sensor, bmi323FifoFrame_t *frame,
                                bmi323AxesData_t *data) {
    bmi323ProcessErrorCode(bmi323_extract_gyro(data, frame, sensor));
}

void bmi323ExtractTemperatureData(bmi323SensorConfiguration_t *sensor, bmi323FifoFrame_t *frame,
                                  bmi323TemperatureData_t *data) {
    bmi323ProcessErrorCode(bmi323_extract_temperature(data, frame, sensor));
}

/* endregion FIFO */

/* region COMMANDS */

void bmi323SendCommand(bmi323SensorConfiguration_t *sensor, uint16_t command) {
    bmi323ProcessErrorCode(bmi323_set_command_register(command, sensor));
}

void bmi323SoftReset(bmi323SensorConfiguration_t *sensor) {
    bmi323ProcessErrorCode(bmi323_soft_reset(sensor));
}

void bmi323PerformSelfTest(bmi323SensorConfiguration_t *sensor, uint8_t features,
                           bmi323TestResults_t *results) {
    bmi323ProcessErrorCode(bmi323_perform_self_test(features, results, sensor));
}

void bmi323PerformGyroscopeSelfCalibration(bmi323SensorConfiguration_t *sensor,
                                           uint8_t selfCalibrationFeature, uint8_t applyCorrection,
                                           bmi323SelfCalibrationResults_t *results) {
    bmi323ProcessErrorCode(
        bmi323_perform_gyro_sc(selfCalibrationFeature, applyCorrection, results, sensor));
}

/* endregion COMMANDS */

/* endregion PUBLIC FUNCTIONS */
