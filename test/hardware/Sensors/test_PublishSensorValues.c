#define SOURCE_FILE "PUBLISH-SENSOR-DATA-TEST"

#include "../hardwareTestHelper.h"
#include "MQTTBroker.h"
#include "TaskWrapper.h"
#include "adxl345b_public.h"
#include "common.h"
#include "pac193x_public.h"
#include "protocol.h"
#include "sht3x_public.h"
#include <hardware/i2c.h>
#include <malloc.h>
#include <stdio.h>

/*!
 * Tries to read out values from the sht3x, pac193x and adxl345b sensors and publishes them
 * over MQTT as DATA under the following data ids.
 * sht3x: temperature / humidity
 * pac193x: energy
 * adxl345b: gValue
 */

#define PAC193X_CHANNEL_SENSORS PAC193X_CHANNEL01

float resistanceValues[4] = {0.82f, 0.82f, 0, 0};
pac193x_usedChannels usedChannels = {.uint_channelsInUse = 0b00000011};

float floatToAbs(float input) {
    if (input < 0) {
        return (-1) * input;
    } else {
        return input;
    }
}

_Bool compareFloatsWithinRange(float expected, float actual, float epsilon) {
    return floatToAbs(expected - actual) <= epsilon;
}

static bool getGValue(float *sumOfAxis) {
    float xAxis = 0, yAxis = 0, zAxis = 0;

    adxl345b_errorCode errorCode = adxl345b_readMeasurements(&xAxis, &yAxis, &zAxis);
    if (errorCode != ADXL345B_NO_ERROR) {
        return false;
    }
    /* 0.2G equals a deviation of about 1% from the ideal value
     * this deviation is given by the datasheet as the accepted tolerance
     * for each axis therefore should epsilon be 0.6G
     */
    *sumOfAxis = floatToAbs(xAxis) + floatToAbs(yAxis) + floatToAbs(zAxis);
    return true;
}

void publishAdxl345bData(void) {
    float sumOfAxis;
    if (getGValue(&sumOfAxis)) {
        PRINT("G value: %f;\n", sumOfAxis)
        int len = snprintf(NULL, 0, "%f", sumOfAxis);
        char *result = malloc(len + 1);
        snprintf(result, len + 1, "%f", sumOfAxis);
        publishData("gValue", result);
        free(result);
    }
}

static bool getValuesOfChannelSensors(pac193x_measurements *measurements) {
    pac193x_errorCode errorCode =
        pac193x_getAllMeasurementsForChannel(PAC193X_CHANNEL_SENSORS, measurements);
    if (errorCode != PAC193X_NO_ERROR)
        return false;

    if (!compareFloatsWithinRange(resistanceValues[0],
                                  measurements->voltageSense / measurements->iSense, 0.1f))
        return false;

    if (!compareFloatsWithinRange(measurements->powerActual,
                                  measurements->iSense * measurements->voltageSource, 0.001f))
        return false;

    return true;
}

void publishPac193xData(void) {
    pac193x_measurements measurements;
    if (getValuesOfChannelSensors(&measurements)) {
        int len = snprintf(NULL, 0, "%f", measurements.energy);
        char *result = malloc(len + 1);
        snprintf(result, len + 1, "%f", measurements.energy);
        publishData("energy", result);
        free(result);
    }
}

static bool getTemperatureAndHumidity(float *temperature, float *humidity) {
    sht3x_errorCode sht_errorCode = sht3x_getTemperatureAndHumidity(temperature, humidity);
    return sht_errorCode == SHT3X_NO_ERROR;
}

void publishSht3xData(void) {
    float temperature = 0, humidity = 0;
    if (getTemperatureAndHumidity(&temperature, &humidity)) {
        int len = snprintf(NULL, 0, "%f", temperature);
        char *result = malloc(len + 1);
        snprintf(result, len + 1, "%f", temperature);
        publishData("temperature", result);
        free(result);

        len = snprintf(NULL, 0, "%f", humidity);
        result = malloc(len + 1);
        snprintf(result, len + 1, "%f", humidity);
        publishData("humidity", result);
        free(result);
    }
}

void _Noreturn mqttTask(void) {
    PRINT("=== STARTING TEST ===")

    sht3x_errorCode sht_errorCode;
    sht_errorCode = sht3x_init(i2c0);
    if (sht_errorCode == SHT3X_NO_ERROR)
        PRINT("Initialised SHT3X\n")
    else
        PRINT("Initialise SHT3X failed; sht3x_ERROR: %02x\n", sht_errorCode)

    pac193x_errorCode pac_errorCode;
    pac_errorCode = pac193x_init(i2c1, resistanceValues, usedChannels);
    if (pac_errorCode == PAC193X_NO_ERROR)
        PRINT("Initialised PAC193X\n")
    else
        PRINT("Initialise PAC193X failed; pac193x_ERROR: %02x\n", sht_errorCode)

    adxl345b_errorCode errorCode;
    errorCode = adxl345b_init(i2c0, ADXL345B_I2C_ALTERNATE_ADDRESS);
    if (errorCode == ADXL345B_NO_ERROR)
        PRINT("Initialised ADXL345B.\n")
    else
        PRINT("Initialise ADXL345B failed; adxl345b_ERROR: %02x\n", errorCode)

    while (true) {
        connectToNetwork();
        connectToMQTT();

        /* TODO: test max number of possible actions */
        publishSht3xData();
        TaskSleep(1000);
        publishPac193xData();
        TaskSleep(1000);
        publishAdxl345bData();
        TaskSleep(2000);
    }
}

int main() {
    initHardwareTest();
    RegisterTask(enterBootModeTaskHardwareTest, "enterBootModeTask");
    RegisterTask(mqttTask, "mqttTask");
    StartScheduler();
}
