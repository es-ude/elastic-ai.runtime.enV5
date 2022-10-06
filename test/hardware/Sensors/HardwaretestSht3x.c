#define SOURCE_FILE "SHT3X-Test"

#include "Common.h"
#include "Sht3x.h"
#include <hardware/i2c.h>
#include <pico/bootrom.h>
#include <pico/stdio_usb.h>
#include <stdio.h>

static void getTemperatureAndHumidity() {
    float temperature, humidity;

    sht3x_errorCode sht_errorCode = sht3x_getTemperatureAndHumidity(&temperature, &humidity);
    if (sht_errorCode == SHT3X_NO_ERROR) {
        PRINT("Temperature: %4.2f°C\tHumidity: %4.2f%%RH", temperature, humidity)
    } else {
        PRINT("ErrorCode: %02X", sht_errorCode)
    }
}

static void getTemperature() {
    float temperature;

    sht3x_errorCode sht_errorCode = sht3x_getTemperature(&temperature);
    if (sht_errorCode == SHT3X_NO_ERROR) {
        PRINT("Temperature: %4.2f°C", temperature)
    } else {
        PRINT("ErrorCode: %02X", sht_errorCode)
    }
}

static void getHumidity() {
    float humidity;

    sht3x_errorCode sht_errorCode = sht3x_getHumidity(&humidity);
    if (sht_errorCode == SHT3X_NO_ERROR) {
        PRINT("Humidity: %4.2f%%RH", humidity)
    } else {
        PRINT("ErrorCode: %02X", sht_errorCode)
    }
}

static void getSerialNumber() {
    uint32_t serialNumber;

    sht3x_errorCode sht_errorCode = sht3x_readSerialNumber(&serialNumber);
    if (sht_errorCode == SHT3X_NO_ERROR) {
        PRINT("Serial number: %li", serialNumber)
    } else {
        PRINT("ErrorCode: %02X", sht_errorCode)
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
            PRINT("Initialise SHT3X")
            break;
        }
        PRINT("Initialise SHT3X failed; sht3x_ERROR: %02X", sht_errorCode)
        sleep_ms(500);
    }

    /* test functions of sht3x */
    PRINT("Please enter a (Temp&Humi), t (Temp), h (Humi), s (serialNo), b "
          "(Boot mode) to perform an action")
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
                  "(serialNo), b (Boot mode) to perform an action")
            break;
        }
    }

    return 0;
}
