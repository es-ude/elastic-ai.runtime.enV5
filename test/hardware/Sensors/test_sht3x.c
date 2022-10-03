//
// Created by David P. Federl
//

#define SOURCE_FILE "SHT3X-Test"

#include "common.h"
#include "sht3x_public.h"
#include <hardware/i2c.h>
#include <pico/bootrom.h>
#include <pico/stdio_usb.h>
#include <stdio.h>

static void getTemperatureAndHumidity() {
    float temperature, humidity;

    sht3x_errorCode sht_errorCode = sht3x_getTemperatureAndHumidity(&temperature, &humidity);
    if (sht_errorCode == SHT3X_NO_ERROR) {
        PRINT("Temperature: %4.2f°C\tHumidity: %4.2f%%RH\n", temperature, humidity)
    } else {
        PRINT("ErrorCode: %i\n", sht_errorCode)
    }
}

static void getTemperature() {
    float temperature;

    sht3x_errorCode sht_errorCode = sht3x_getTemperature(&temperature);
    if (sht_errorCode == SHT3X_NO_ERROR) {
        PRINT("Temperature: %4.2f°C\n", temperature)
    } else {
        PRINT("ErrorCode: %i\n", sht_errorCode)
    }
}

static void getHumidity() {
    float humidity;

    sht3x_errorCode sht_errorCode = sht3x_getHumidity(&humidity);
    if (sht_errorCode == SHT3X_NO_ERROR) {
        PRINT("Humidity: %4.2f%%RH\n", humidity)
    } else {
        PRINT("ErrorCode: %i\n", sht_errorCode)
    }
}

static void getSerialNumber() {
    uint32_t serialNumber;

    sht3x_errorCode sht_errorCode = sht3x_readSerialNumber(&serialNumber);
    if (sht_errorCode == SHT3X_NO_ERROR) {
        PRINT("Serial number: %li\n", serialNumber)
    } else {
        PRINT("ErrorCode: %i\r\n", sht_errorCode)
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

    /* initialize SHT3X sensor */
    PRINT("START INIT")
    sht3x_errorCode sht_errorCode;
    while (1) {
        sht_errorCode = sht3x_init(i2c0);
        if (sht_errorCode == SHT3X_NO_ERROR) {
            PRINT("Initialise SHT3X\n")
            break;
        }
        PRINT("Initialise SHT3X failed; sht3x_ERROR: %02x\n", sht_errorCode)
        sleep_ms(500);
    }

    /* test functions of sht3x */
    PRINT("Please enter a (Temp&Humi), t (Temp), h (Humi), s (serialNo), b "
          "(Boot mode) to perform an action\n")
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
                  "(serialNo), b (Boot mode) to perform an action\n")
            break;
        }
    }

    return 0;
}
