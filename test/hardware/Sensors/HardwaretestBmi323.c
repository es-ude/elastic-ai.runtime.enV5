#define SOURCE_FILE "HW-TEST-BMI323"

#include "hardware/spi.h"
#include "pico/bootrom.h"
#include "pico/stdio.h"
#include "pico/stdio_usb.h"

#include "Common.h"
#include "EnV5HwConfiguration.h"
#include "EnV5HwController.h"
#include "Sleep.h"
#include "Spi.h"
#include "SpiTypedefs.h"
#include "bmi323.h"
#include "bmi323_defs.h"
#include "bmi323_interface.h"

#define LIMIT 100

spiConfiguration_t spi = {
    .spiInstance = BMI323_SPI_INTERFACE,
    .misoPin = BMI323_SPI_MISO,
    .mosiPin = BMI323_SPI_MOSI,
    .sckPin = BMI323_SPI_CLOCK,
    .csPin = BMI323_SPI_CHIP_SELECT,
    .baudrate = BMI323_SPI_BAUDRATE,
};

struct bmi3_dev bmi323 = {0};

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

    int8_t errorCode;
    errorCode = bmi323Init(&bmi323, &spi);
    if (BMI323_OK != errorCode) {
        PRINT("ERROR: %i", errorCode);
        gotoBootloader();
    }
    PRINT("BMI323 initialized");
}

static void getChipId(void) {
    uint8_t chipId[2];
    int8_t result = bmi323_get_regs(BMI3_REG_CHIP_ID, chipId, 2, &bmi323);
    if (BMI3_OK != result) {
        PRINT("ERROR: %i", result);
        gotoBootloader();
    }
    PRINT("Chip-ID: 0x%04X", chipId[0] << 8 | chipId[1]);
}
static void initializeGyroscope(void) {
    struct bmi3_map_int mapInterrupt = {0};
    int8_t errorCode;

    struct bmi3_sens_config config = {0};
    config.type = BMI323_GYRO;
    errorCode = bmi323_get_sensor_config(&config, 1, &bmi323);
    if (BMI3_OK != errorCode) {
        PRINT("Error: %i", errorCode);
        gotoBootloader();
    }

    mapInterrupt.gyr_drdy_int = BMI3_INT1;
    errorCode = bmi323_map_interrupt(mapInterrupt, &bmi323);
    if (BMI3_OK != errorCode) {
        PRINT("Error: %i", errorCode);
        gotoBootloader();
    }

    config.cfg.gyr.odr = BMI3_GYR_ODR_100HZ;
    config.cfg.gyr.range = BMI3_GYR_RANGE_2000DPS;
    config.cfg.gyr.bwp = BMI3_GYR_BW_ODR_HALF;
    config.cfg.gyr.gyr_mode = BMI3_GYR_MODE_NORMAL;
    config.cfg.gyr.avg_num = BMI3_GYR_AVG1;
    errorCode = bmi323_set_sensor_config(&config, 1, &bmi323);
    if (BMI3_OK != errorCode) {
        PRINT("Error: %i", errorCode);
        gotoBootloader();
    }
}
static void getGyroscopeData() { // TODO: can't receive data
    struct bmi3_sensor_data sensorData = {0};
    sensorData.type = BMI323_GYRO;

    uint8_t index = 0;
    while (index <= LIMIT) {
        uint16_t status = 0;
        int8_t errorCode = bmi323_get_sensor_status(&status, &bmi323);
        if (BMI3_OK != errorCode) {
            PRINT("ERROR: %i", errorCode);
            continue;
        }
        PRINT_DEBUG("Status: 0x%04X", status);

        uint16_t interruptState = 0;
        errorCode = bmi323_get_int1_status(&interruptState, &bmi323);
        if (BMI3_OK != errorCode) {
            PRINT("ERROR: %i", errorCode);
            continue;
        }
        PRINT_DEBUG("Interrupt State: 0x%04X", interruptState);

        if (interruptState & BMI3_INT_STATUS_GYR_DRDY) {
            errorCode = bmi323_get_sensor_data(&sensorData, 1, &bmi323);
            if (BMI3_OK != errorCode) {
                PRINT("ERROR: %i", errorCode);
                continue;
            }

            float x = bmi323LsbToDps(sensorData.sens_data.gyr.x, BMI3_GYR_RANGE_2000DPS,
                                     bmi323.resolution);
            float y = bmi323LsbToDps(sensorData.sens_data.gyr.y, BMI3_GYR_RANGE_2000DPS,
                                     bmi323.resolution);
            float z = bmi323LsbToDps(sensorData.sens_data.gyr.z, BMI3_GYR_RANGE_2000DPS,
                                     bmi323.resolution);

            PRINT("%d:\n"
                  "\tRange=%dDPS\n"
                  "\tRaw: X=%i, Y=%i, Z=%i\n"
                  "\tDPS: X=%4.2f, Y=%4.2f, Z=%4.2f",
                  index, 2000, sensorData.sens_data.gyr.x, sensorData.sens_data.gyr.y,
                  sensorData.sens_data.gyr.z, x, y, z);

            index++;
        }
    }
}
static void testGyroscope(void) {
    initializeGyroscope();
    getGyroscopeData(100);
}

static void initializeTemperature(void) { // TODO: get config -> Error -4
    struct bmi3_sens_config config = {0};
    int errorCode = bmi323_get_sensor_config(&config, 1, &bmi323);
    if (BMI3_OK != errorCode) {
        PRINT("ERROR: %i", errorCode);
        gotoBootloader();
    }

    config.type = BMI3_ACCEL;
    config.cfg.acc.acc_mode = BMI3_ACC_MODE_NORMAL;
    errorCode = bmi323_set_sensor_config(&config, 1, &bmi323);
    if (BMI3_OK != errorCode) {
        PRINT("ERROR: %i", errorCode);
        gotoBootloader();
    }
    PRINT("Sensor Configured");

    struct bmi3_map_int map_int = {0};
    map_int.temp_drdy_int = BMI3_INT1;
    errorCode = bmi323_map_interrupt(map_int, &bmi323);
    if (BMI3_OK != errorCode) {
        PRINT("ERROR: %i", errorCode);
        gotoBootloader();
    }
    PRINT("Interrupt Enabled");
}
static void getTemperatureData() {
    uint8_t index = 0;
    while (index <= LIMIT) {
        uint16_t interruptStatus = 0;
        int errorCode = bmi323_get_int1_status(&interruptStatus, &bmi323);
        if (BMI323_OK != errorCode) {
            PRINT("ERROR: %i", errorCode);
            continue;
        }

        if (interruptStatus & BMI3_INT_STATUS_TEMP_DRDY) {
            uint16_t rawTemperature;
            errorCode = bmi323_get_temperature_data(&rawTemperature, &bmi323);
            if (BMI323_OK != errorCode) {
                PRINT("ERROR: %i", errorCode);
                continue;
            }

            float temperature_value = (float)((((float)((int16_t)rawTemperature)) / 512.0) + 23.0);

            uint32_t sensorTime = 0;
            errorCode = bmi323_get_sensor_time(&sensorTime, &bmi323);
            if (BMI323_OK != errorCode) {
                PRINT("ERROR: %i", errorCode);
                continue;
            }

            PRINT("%04i (%.4lfs): %f°C (%u)", index, (sensorTime * BMI3_SENSORTIME_RESOLUTION),
                  temperature_value, rawTemperature);

            index++;
        }
    }
}
static void testTemperature(void) {
    initializeTemperature();
    getTemperatureData(10);
}

/* This function starts the execution of program. */
int main(void) {
    initializeCommunication();

    PRINT("===== START TEST =====");
    initializeBmi323();
    getChipId();
    //    testTemperature();
    testGyroscope();

    gotoBootloader();

    return -1;
}
