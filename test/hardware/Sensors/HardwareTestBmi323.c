#define SOURCE_FILE "HW-TEST-BMI323"

#include "hardware/spi.h"
#include "pico/bootrom.h"
#include "pico/stdio.h"
#include "pico/stdio_usb.h"

#include "eai/Common.h"
#include "eai/hal/EnV5HwConfiguration.h"
#include "eai/hal/EnV5HwController.h"
#include "eai/hal/Sleep.h"
#include "eai/hal/Spi.h"
#include "eai/hal/SpiTypedefs.h"
#include "eai/sensor/Bmi323.h"

#define LIMIT (100)

spiConfiguration_t spi = {
    .spiInstance = BMI323_SPI_MODULE,
    .misoPin = BMI323_SPI_MISO,
    .mosiPin = BMI323_SPI_MOSI,
    .sckPin = BMI323_SPI_CLOCK,
    .csPin = BMI323_SPI_CHIP_SELECT,
    .baudrate = BMI323_SPI_BAUDRATE,
};

bmi323SensorConfiguration_t bmi323[1];

static void initializeCommunication(void) {
    env5HwControllerInit();

    stdio_init_all();
    while (!stdio_usb_connected()) {
        // Wait for connection
    }
    sleep_for_ms(500);
    PRINT_DEBUG("CDC established");
}

static void gotoBootloader(void) {
    reset_usb_boot(0, 0);
}

static void initializeBmi323(void) {
    bmi323ErrorCode_t errorCode;
    spiInit(&spi);
    PRINT_DEBUG("SPI initialized ");

    while (1) {
        errorCode = bmi323Init(bmi323, &spi);
        if (errorCode != BMI323_OK) {
            PRINT("\033Initialise bmi323 failed\033; bmi323_ERROR: %02X", errorCode);
            sleep_ms(500);
            continue;
        }
    }
    PRINT("BMI323 initialized");
}

static void getChipId(void) {
    bmi323ErrorCode_t errorCode;
        uint8_t chipId[2];
        data_t idData = {.data = chipId, .length = sizeof(chipId)};
        errorCode = bmi323GetRegister(bmi323, BMI3_REG_CHIP_ID, &idData);
        if(errorCode != BMI323_OK){
            PRINT("\033[0;31mFAILED\033[0m; bmi323_ERROR: %02X", errorCode);
            return;
        }
        PRINT("Chip-ID: 0x%04X", chipId[0] << 8 | chipId[1]);
        return;
}

static void initializeAccelerometer(void) {
    bmi323ErrorCode_t errorCode;
        bmi323InterruptMapping_t mapping = {.acc_drdy_int = BMI323_INTERRUPT_1};
        bmi323SetInterruptMapping(bmi323, mapping);
        if(errorCode != BMI323_OK){
            PRINT("\033[0;31mFAILED\033[0m; bmi323_ERROR: %02X", errorCode);
            return;
        }
        PRINT("Interrupt Mapped");

        bmi323FeatureConfiguration_t config[1];
        config[0].type = BMI323_ACCEL;
        bmi323GetSensorConfiguration(bmi323, 1, config);
        if(errorCode != BMI323_OK){
            PRINT("\033[0;31mFAILED\033[0m; bmi323_ERROR: %02X", errorCode);
            return;
        }
        config[0].cfg.acc.odr = BMI3_ACC_ODR_100HZ;
        config[0].cfg.acc.range = BMI3_ACC_RANGE_16G;
        config[0].cfg.gyr.bwp = BMI3_ACC_BW_ODR_QUARTER;
        config[0].cfg.gyr.avg_num = BMI3_ACC_AVG1;
        config[0].cfg.gyr.gyr_mode = BMI3_ACC_MODE_NORMAL;
        bmi323SetSensorConfiguration(bmi323, 1, config);
        if(errorCode != BMI323_OK){
            PRINT("\033[0;31mFAILED\033[0m; bmi323_ERROR: %02X", errorCode);
            return;
        }
        PRINT("Accelerometer configured");
}
static void getAccelerometerData() {
    bmi323ErrorCode_t errorCode;
    uint16_t interrupt;
    uint8_t index = 0;
    while (index <= LIMIT) {
            errorCode = bmi323GetInterruptStatus(bmi323, BMI323_INTERRUPT_1, &interrupt);
            if(errorCode != BMI323_OK){
                PRINT("\033[0;31mFAILED\033[0m; bmi323_ERROR: %02X", errorCode);
                return;
            }
            if (interrupt & BMI3_INT_STATUS_ACC_DRDY) {
                bmi323SensorData_t data[1];
                data[0].type = BMI323_ACCEL;
                bmi323GetData(bmi323, 1, data);
                if(errorCode != BMI323_OK){
                    PRINT("\033[0;31mFAILED\033[0m; bmi323_ERROR: %02X", errorCode);
                    return;
                }
                float x, y, z;
                errorCode = bmi323LsbToMps2(data[0].sens_data.acc.x, BMI3_ACC_RANGE_16G,
                                          bmi323[0].resolution, &x);
                if(errorCode != BMI323_OK){
                    PRINT("\033[0;31mFAILED\033[0m; bmi323_ERROR: %02X", errorCode);
                    return;
                }
                errorCode = bmi323LsbToMps2(data[0].sens_data.acc.y, BMI3_ACC_RANGE_16G,
                                          bmi323[0].resolution, &y);
                if(errorCode != BMI323_OK){
                    PRINT("\033[0;31mFAILED\033[0m; bmi323_ERROR: %02X", errorCode);
                    return;
                }
                errorCode = bmi323LsbToMps2(data[0].sens_data.acc.z, BMI3_ACC_RANGE_16G,
                                          bmi323[0].resolution, &z);
                if(errorCode != BMI323_OK){
                    PRINT("\033[0;31mFAILED\033[0m; bmi323_ERROR: %02X", errorCode);
                    return;
                }

                PRINT("%d:\n"
                      "\tRange=%dg\n"
                      "\tRaw: X=0x%X, Y=0x%X, Z=0x%X\n"
                      "\tMPS2: X=%4.2f, Y=%4.2f, Z=%4.2f",
                      index, 16, data[0].sens_data.acc.x, data[0].sens_data.acc.y,
                      data[0].sens_data.acc.z, x, y, z);

                index++;
            }
    }
}
static void testAccelerometer(void) {
    initializeAccelerometer();
    getAccelerometerData();
}

static void initializeGyroscope(void) {
    bmi323ErrorCode_t errorCode;
        bmi323FeatureConfiguration_t config[0];
        config[0].type = BMI323_GYRO;
        errorCode = bmi323GetSensorConfiguration(bmi323, 1, config);
        if(errorCode != BMI323_OK){
            PRINT("\033[0;31mFAILED\033[0m; bmi323_ERROR: %02X", errorCode);
            return;
        }
        config[0].cfg.gyr.odr = BMI3_GYR_ODR_100HZ;
        config[0].cfg.gyr.range = BMI3_GYR_RANGE_2000DPS;
        config[0].cfg.gyr.bwp = BMI3_GYR_BW_ODR_HALF;
        config[0].cfg.gyr.gyr_mode = BMI3_GYR_MODE_NORMAL;
        config[0].cfg.gyr.avg_num = BMI3_GYR_AVG1;
        errorCode = bmi323SetSensorConfiguration(bmi323, 1, config);
        if(errorCode != BMI323_OK){
            PRINT("\033[0;31mFAILED\033[0m; bmi323_ERROR: %02X", errorCode);
            return;
        }
        PRINT("Gyroscope configured");

        bmi323InterruptMapping_t mapping = {.gyr_drdy_int = BMI323_INTERRUPT_1};
        errorCode = bmi323SetInterruptMapping(bmi323, mapping);
        if(errorCode != BMI323_OK){
            PRINT("\033[0;31mFAILED\033[0m; bmi323_ERROR: %02X", errorCode);
            return;
        }
        PRINT("Interrupt Mapped");

        bmi323SelfCalibrationResults_t calibration;
        errorCode = bmi323PerformGyroscopeSelfCalibration(bmi323, BMI3_SC_OFFSET_EN, BMI3_SC_APPLY_CORR_EN,
                                              &calibration);
        if(errorCode != BMI323_OK){
            PRINT("\033[0;31mFAILED\033[0m; bmi323_ERROR: %02X", errorCode);
            return;
        }
        PRINT("Calibration Results: %X", calibration.gyro_sc_rslt);
}
static void getGyroscopeData() {
    bmi323ErrorCode_t errorCode;
    uint16_t interrupt;
    uint8_t index = 0;
    while (index <= LIMIT) {
            errorCode = bmi323GetInterruptStatus(bmi323, BMI323_INTERRUPT_1, interrupt);
            if(errorCode != BMI323_OK){
                PRINT("\033[0;31mFAILED\033[0m; bmi323_ERROR: %02X", errorCode);
                return;
            }
            if (!(interrupt & BMI3_INT_STATUS_GYR_DRDY)) {
                continue;
            }

            bmi323SensorData_t data[1];
            data[0].type = BMI323_GYRO;
            errorCode = bmi323GetData(bmi323, 1, data);
            if(errorCode != BMI323_OK){
                PRINT("\033[0;31mFAILED\033[0m; bmi323_ERROR: %02X", errorCode);
                return;
            }
            float x,y,z;
            errorCode = bmi323LsbToDps(data[0].sens_data.gyr.x, BMI3_GYR_RANGE_2000DPS,
                                     bmi323[0].resolution);
            if(errorCode != BMI323_OK){
                PRINT("\033[0;31mFAILED\033[0m; bmi323_ERROR: %02X", errorCode);
                return;
            }
            errorCode = bmi323LsbToDps(data[0].sens_data.gyr.y, BMI3_GYR_RANGE_2000DPS,
                                     bmi323[0].resolution);
            if(errorCode != BMI323_OK){
                PRINT("\033[0;31mFAILED\033[0m; bmi323_ERROR: %02X", errorCode);
                return;
            }
            errorCode = bmi323LsbToDps(data[0].sens_data.gyr.z, BMI3_GYR_RANGE_2000DPS,
                                     bmi323[0].resolution);
            if(errorCode != BMI323_OK){
                PRINT("\033[0;31mFAILED\033[0m; bmi323_ERROR: %02X", errorCode);
                return;
            }

            PRINT("%d:\n"
                  "\tRange=%dDPS\n"
                  "\tRaw: X=0x%X, Y=0x%X, Z=0x%X\n"
                  "\tDPS: X=%4.2f, Y=%4.2f, Z=%4.2f",
                  index, 2000, data[0].sens_data.gyr.x, data[0].sens_data.gyr.y,
                  data[0].sens_data.gyr.z, x, y, z);

            index++;
    }
}
static void testGyroscope(void) {
    initializeGyroscope();
    getGyroscopeData();
}

static void initializeTemperature(void) {
    bmi323ErrorCode_t errorCode;
        bmi323InterruptMapping_t mapping = {.temp_drdy_int = BMI323_INTERRUPT_1};
        errorCode = bmi323SetInterruptMapping(bmi323, mapping);
        if(errorCode != BMI323_OK){
            PRINT("\033[0;31mFAILED\033[0m; bmi323_ERROR: %02X", errorCode);
            return;
        }
        PRINT("Interrupt Enabled");

        bmi323FeatureConfiguration_t config[1];
        config[0].type = BMI3_ACCEL;
        errorCode = bmi323GetSensorConfiguration(bmi323, 1, config);
        if(errorCode != BMI323_OK){
            PRINT("\033[0;31mFAILED\033[0m; bmi323_ERROR: %02X", errorCode);
            return;
        }
        config[0].cfg.acc.acc_mode = BMI3_ACC_MODE_NORMAL;
        errorCode = bmi323SetSensorConfiguration(bmi323, 1, config);
        if(errorCode != BMI323_OK){
            PRINT("\033[0;31mFAILED\033[0m; bmi323_ERROR: %02X", errorCode);
            return;
        }
        PRINT("Sensor Configured");
}
static void getTemperatureData() {
    bmi323ErrorCode_t errorCode;
    uint16_t interruptStatus;
    float *temperature;
    float *sensorTime;
    uint8_t index = 0;
    while (index <= LIMIT) {
            errorCode = bmi323GetInterruptStatus(bmi323, BMI323_INTERRUPT_1, interruptStatus);
            if (interruptStatus & BMI3_INT_STATUS_TEMP_DRDY) {
                errorCode = bmi323GetTemperature(bmi323, temperature);
                if(errorCode != BMI323_OK){
                    PRINT("\033[0;31mFAILED\033[0m; bmi323_ERROR: %02X", errorCode);
                    return;
                }
                errorCode = bmi323GetSensorTime(bmi323, sensorTime);
                if(errorCode != BMI323_OK){
                    PRINT("\033[0;31mFAILED\033[0m; bmi323_ERROR: %02X", errorCode);
                    return;
                }
                PRINT("%04i (%.4fs): %f°C", index, sensorTime, temperature);

                index++;
            }
    }
}
static void testTemperature(void) {
    initializeTemperature();
    getTemperatureData();
}

/* This function starts the execution of program. */
int main(void) {
    initializeCommunication();

    PRINT("===== START TEST =====");
    initializeBmi323();
    getChipId();

    testAccelerometer();
    testTemperature();
    testGyroscope();

    gotoBootloader();

    return -1;
}
