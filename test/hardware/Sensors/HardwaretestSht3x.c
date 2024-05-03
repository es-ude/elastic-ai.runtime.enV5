#define SOURCE_FILE "SHT3X-Test"

#include "Common.h"
#include "Sht3x.h"
#include <hardware/i2c.h>
#include <pico/bootrom.h>
#include <pico/stdio_usb.h>
#include <stdio.h>
#include "I2c.h"

/* region I2C DEFINITION */
i2cConfiguration_t i2cConfig = {
    .i2cInstance = i2c1,
    .frequency = 400000,
    .sdaPin = 6,
    .sclPin = 7,
};
/* endregion I2C DEFINITION */


/* region SENSOR DEFINITION */
static sht3xSensorConfiguration_t sensor = {
    .i2c_slave_address = SHT3X_I2C_ADDRESS,
    .i2c_host = i2c1,
};
/* endregion SENSOR DEFINITION */


static void getTemperatureAndHumidity() {
    float temperature, humidity;

    sht3xErrorCode_t sht_errorCode = sht3xGetTemperatureAndHumidity(sensor, &temperature, &humidity);
    if (sht_errorCode == SHT3X_NO_ERROR) {
        PRINT("Temperature: %4.2f°C\tHumidity: %4.2f%%RH", temperature, humidity);
    } else {
        PRINT("ErrorCode: %02X", sht_errorCode);
    }
}

static void getTemperature() {
    float temperature;

    sht3xErrorCode_t sht_errorCode = sht3xGetTemperature(sensor, &temperature);
    if (sht_errorCode == SHT3X_NO_ERROR) {
        PRINT("Temperature: %4.2f°C", temperature);
    } else {
        PRINT("ErrorCode: %02X", sht_errorCode);
    }
}

static void getHumidity() {
    float humidity;

    sht3xErrorCode_t sht_errorCode = sht3xGetHumidity(sensor, &humidity);
    if (sht_errorCode == SHT3X_NO_ERROR) {
        PRINT("Humidity: %4.2f%%RH", humidity);
    } else {
        PRINT("ErrorCode: %02X", sht_errorCode);
    }
}

static void getSerialNumber() {
    uint32_t serialNumber;

    sht3xErrorCode_t sht_errorCode = sht3xReadSerialNumber(sensor, &serialNumber);
    if (sht_errorCode == SHT3X_NO_ERROR) {
        PRINT("Serial number: %li", serialNumber);
    } else {
        PRINT("ErrorCode: %02X", sht_errorCode);
    }
}

static void enterBootMode() {
    reset_usb_boot(0, 0);
}

int main(void) {
    /* enable print to console output */
    stdio_init_all();
    // wait for user console to connect
    while ((!stdio_usb_connected())) {}
    sleep_ms(500);

    /* initialize I2C */
    PRINT("===== START I2C INIT =====");
    i2cErrorCode_t i2cErrorCode;
    while(1) {
        i2cErrorCode = i2cInit(&i2cConfig);
        if (i2cErrorCode == I2C_NO_FREQUENCY_ERROR_OTHER_UNKNOWN_YET){
            PRINT("Initialised I2C.");
            break;
        }
        PRINT("Initialise I2C failed; i2c_ERROR: %02X", i2cErrorCode);
        sleep_ms(500);
    }
    
    /* initialize SHT3X sensor */
    PRINT("===== START INIT =====");
    sht3xErrorCode_t sht_errorCode;
    while (1) {
        sht_errorCode = sht3xInit(sensor);
        if (sht_errorCode == SHT3X_NO_ERROR) {
            PRINT("Initialise SHT3X");
            break;
        }
        PRINT("Initialise SHT3X failed; sht3x_ERROR: %02X", sht_errorCode);
        sleep_ms(500);
    }

    /* test functions of sht3x */
    PRINT("===== START TEST =====");
    PRINT("Please enter a (Temp&Humi), t (Temp), h (Humi), s (serialNo), b "
          "(Boot mode) to perform an action");
    while (1) {
        char input = getchar_timeout_us(10000000);

        switch (input) {
        case 'a':
            getTemperatureAndHumidity();
            break;
        case 't':
            getTemperature();
            break;
        case 'h':
            getHumidity();
            break;
        case 's':
            getSerialNumber();
            break;
        case 'b':
            enterBootMode();
            break;
        default:
            PRINT("Please enter a (Temp&Humi), t (Temp), h (Humi), s "
                  "(serialNo), b (Boot mode) to perform an action");
            break;
        }
    }

    return 0;
}
