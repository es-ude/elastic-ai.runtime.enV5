#define SOURCE_FILE "HW-TEST-BMI323"

#include "hardware/spi.h"
#include "pico/bootrom.h"
#include "pico/stdio.h"
#include "pico/stdio_usb.h"

#include "CException.h"

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
    spiInit(&spi);
    PRINT_DEBUG("Initialized SPI");

    CEXCEPTION_T exception;
    Try {
        bmi323Init(bmi323, &spi);
    }
    Catch(exception) {
        PRINT("ERROR: %i", exception);
        gotoBootloader();
    }
    PRINT("BMI323 initialized");
}

static void getChipId(void) {
    CEXCEPTION_T exception;
    Try {
        uint8_t chipId[2];
        data_t idData = {.data = chipId, .length = sizeof(chipId)};
        bmi323GetRegister(bmi323, BMI3_REG_CHIP_ID, &idData);
        PRINT("Chip-ID: 0x%04X", chipId[0] << 8 | chipId[1]);
    }
    Catch(exception) {
        PRINT("ERROR: %i", exception);
        gotoBootloader();
    }
}

static void initializeAccelerometer(void) {
    CEXCEPTION_T exception;
    Try {
        bmi323InterruptMapping_t mapping = {.acc_drdy_int = BMI323_INTERRUPT_1};
        bmi323SetInterruptMapping(bmi323, mapping);
        PRINT("Interrupt Mapped");

        bmi323FeatureConfiguration_t config[1];
        config[0].type = BMI323_ACCEL;
        bmi323GetSensorConfiguration(bmi323, 1, config);
        config[0].cfg.acc.odr = BMI3_ACC_ODR_100HZ;
        config[0].cfg.acc.range = BMI3_ACC_RANGE_16G;
        config[0].cfg.gyr.bwp = BMI3_ACC_BW_ODR_QUARTER;
        config[0].cfg.gyr.avg_num = BMI3_ACC_AVG1;
        config[0].cfg.gyr.gyr_mode = BMI3_ACC_MODE_NORMAL;
        bmi323SetSensorConfiguration(bmi323, 1, config);
        PRINT("Accelerometer configured");
    }
    Catch(exception) {
        PRINT("Error: %i", exception);
        gotoBootloader();
    }
}
static void getAccelerometerData() {
    CEXCEPTION_T exception;
    uint8_t index = 0;
    while (index <= LIMIT) {
        Try {
            uint16_t interrupt = bmi323GetInterruptStatus(bmi323, BMI323_INTERRUPT_1);
            if (interrupt & BMI3_INT_STATUS_ACC_DRDY) {
                bmi323SensorData_t data[1];
                data[0].type = BMI323_ACCEL;
                bmi323GetData(bmi323, 1, data);

                float x = bmi323LsbToMps2(data[0].sens_data.acc.x, BMI3_ACC_RANGE_16G,
                                          bmi323[0].resolution);
                float y = bmi323LsbToMps2(data[0].sens_data.acc.y, BMI3_ACC_RANGE_16G,
                                          bmi323[0].resolution);
                float z = bmi323LsbToMps2(data[0].sens_data.acc.z, BMI3_ACC_RANGE_16G,
                                          bmi323[0].resolution);

                PRINT("%d:\n"
                      "\tRange=%dg\n"
                      "\tRaw: X=0x%X, Y=0x%X, Z=0x%X\n"
                      "\tMPS2: X=%4.2f, Y=%4.2f, Z=%4.2f",
                      index, 16, data[0].sens_data.acc.x, data[0].sens_data.acc.y,
                      data[0].sens_data.acc.z, x, y, z);

                index++;
            }
        }
        Catch(exception) {
            PRINT("ERROR: %i", exception);
        }
    }
}
static void testAccelerometer(void) {
    initializeAccelerometer();
    getAccelerometerData();
}

static void initializeGyroscope(void) {
    CEXCEPTION_T exception;
    Try {
        bmi323FeatureConfiguration_t config[0];
        config[0].type = BMI323_GYRO;
        bmi323GetSensorConfiguration(bmi323, 1, config);
        config[0].cfg.gyr.odr = BMI3_GYR_ODR_100HZ;
        config[0].cfg.gyr.range = BMI3_GYR_RANGE_2000DPS;
        config[0].cfg.gyr.bwp = BMI3_GYR_BW_ODR_HALF;
        config[0].cfg.gyr.gyr_mode = BMI3_GYR_MODE_NORMAL;
        config[0].cfg.gyr.avg_num = BMI3_GYR_AVG1;
        bmi323SetSensorConfiguration(bmi323, 1, config);
        PRINT("Gyroscope configured");

        bmi323InterruptMapping_t mapping = {.gyr_drdy_int = BMI323_INTERRUPT_1};
        bmi323SetInterruptMapping(bmi323, mapping);
        PRINT("Interrupt Mapped");

        bmi323SelfCalibrationResults_t calibration;
        bmi323PerformGyroscopeSelfCalibration(bmi323, BMI3_SC_OFFSET_EN, BMI3_SC_APPLY_CORR_EN,
                                              &calibration);
        PRINT("Calibration Results: %X", calibration.gyro_sc_rslt);
    }
    Catch(exception) {
        PRINT("Error: %i", exception);
        gotoBootloader();
    }
}
static void getGyroscopeData() {
    CEXCEPTION_T exception;
    uint8_t index = 0;
    while (index <= LIMIT) {
        Try {
            uint16_t interrupt = bmi323GetInterruptStatus(bmi323, BMI323_INTERRUPT_1);
            if (!(interrupt & BMI3_INT_STATUS_GYR_DRDY)) {
                continue;
            }

            bmi323SensorData_t data[1];
            data[0].type = BMI323_GYRO;
            bmi323GetData(bmi323, 1, data);

            float x = bmi323LsbToDps(data[0].sens_data.gyr.x, BMI3_GYR_RANGE_2000DPS,
                                     bmi323[0].resolution);
            float y = bmi323LsbToDps(data[0].sens_data.gyr.y, BMI3_GYR_RANGE_2000DPS,
                                     bmi323[0].resolution);
            float z = bmi323LsbToDps(data[0].sens_data.gyr.z, BMI3_GYR_RANGE_2000DPS,
                                     bmi323[0].resolution);

            PRINT("%d:\n"
                  "\tRange=%dDPS\n"
                  "\tRaw: X=0x%X, Y=0x%X, Z=0x%X\n"
                  "\tDPS: X=%4.2f, Y=%4.2f, Z=%4.2f",
                  index, 2000, data[0].sens_data.gyr.x, data[0].sens_data.gyr.y,
                  data[0].sens_data.gyr.z, x, y, z);

            index++;
        }
        Catch(exception) {
            PRINT("ERROR: %i", exception);
        }
    }
}
static void testGyroscope(void) {
    initializeGyroscope();
    getGyroscopeData();
}

static void initializeTemperature(void) {
    CEXCEPTION_T exception;
    Try {
        bmi323InterruptMapping_t mapping = {.temp_drdy_int = BMI323_INTERRUPT_1};
        bmi323SetInterruptMapping(bmi323, mapping);
        PRINT("Interrupt Enabled");

        bmi323FeatureConfiguration_t config[1];
        config[0].type = BMI3_ACCEL;
        bmi323GetSensorConfiguration(bmi323, 1, config);
        config[0].cfg.acc.acc_mode = BMI3_ACC_MODE_NORMAL;
        bmi323SetSensorConfiguration(bmi323, 1, config);
        PRINT("Sensor Configured");
    }
    Catch(exception) {
        PRINT("ERROR: %i", exception);
        gotoBootloader();
    }
}
static void getTemperatureData() {
    uint8_t index = 0;
    while (index <= LIMIT) {
        CEXCEPTION_T exception;
        Try {
            uint16_t interruptStatus = bmi323GetInterruptStatus(bmi323, BMI323_INTERRUPT_1);
            if (interruptStatus & BMI3_INT_STATUS_TEMP_DRDY) {
                float temperature = bmi323GetTemperature(bmi323);
                float sensorTime = bmi323GetSensorTime(bmi323);
                PRINT("%04i (%.4fs): %f°C", index, sensorTime, temperature);

                index++;
            }
        }
        Catch(exception) {
            PRINT("ERROR: %i", exception);
            continue;
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
