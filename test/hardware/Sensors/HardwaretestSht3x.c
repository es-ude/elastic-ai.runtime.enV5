#define SOURCE_FILE "SHT3X-Test"

#include <stdio.h>

#include "CException.h"
#include "hardware/i2c.h"
#include "pico/bootrom.h"
#include "pico/stdio_usb.h"

#include "Common.h"
#include "EnV5HwConfiguration.h"
#include "EnV5HwController.h"
#include "I2c.h"
#include "Sht3x.h"

/* region I2C DEFINITION */
i2cConfiguration_t i2cConfig = {
    .i2cInstance = I2C_INSTANCE,
    .frequency = I2C_FREQUENCY_IN_HZ,
    .sdaPin = I2C_SDA_PIN,
    .sclPin = I2C_SCL_PIN,
};
/* endregion I2C DEFINITION */
/* region SENSOR DEFINITION */
static sht3xSensorConfiguration_t sensor = {
    .i2c_host = SHT_HOST,
    .i2c_slave_address = SHT_SLAVE,
};
/* endregion SENSOR DEFINITION */

static void getTemperatureAndHumidity(void) {
    CEXCEPTION_T exception;
    Try {
        float temperature, humidity;
        sht3xGetTemperatureAndHumidity(sensor, &temperature, &humidity);
        PRINT("Temperature: %4.2f°C\tHumidity: %4.2f%%RH", temperature, humidity);
    }
    Catch(exception) {
        PRINT("ErrorCode: %02X", exception);
    }
}

static void getTemperature(void) {
    CEXCEPTION_T exception;
    Try {
        float temperature = sht3xGetTemperature(sensor);
        PRINT("Temperature: %4.2f°C", temperature);
    }
    Catch(exception) {
        PRINT("ErrorCode: %02X", exception);
    }
}

static void getHumidity(void) {
    CEXCEPTION_T exception;
    Try {
        float humidity = sht3xGetHumidity(sensor);
        PRINT("Humidity: %4.2f%%RH", humidity);
    }
    Catch(exception) {
        PRINT("ErrorCode: %02X", exception);
    }
}

static void getSerialNumber(void) {
    CEXCEPTION_T exception;
    Try {
        uint32_t serialNumber = sht3xReadSerialNumber(sensor);
        PRINT("Serial number: %li", serialNumber);
    }
    Catch(exception) {
        PRINT("ErrorCode: %02X", exception);
    }
}

static void enterBootMode(void) {
    reset_usb_boot(0, 0);
}

static void initHardware(void) {
    env5HwControllerInit();

    /* enable print to console output */
    stdio_init_all();
    // wait for user console to connect
    while ((!stdio_usb_connected())) {}
    sleep_ms(500);

    /* initialize I2C */
    PRINT("===== START I2C INIT =====");
    i2cErrorCode_t i2cErrorCode;
    while (1) {
        i2cErrorCode = i2cInit(&i2cConfig);
        if (i2cErrorCode == I2C_NO_ERROR) {
            PRINT("Initialised I2C.");
            break;
        }
        PRINT("Initialise I2C failed; i2c_ERROR: %02X", i2cErrorCode);
        sleep_ms(500);
    }

    /* initialize SHT3X sensor */
    PRINT("===== START INIT =====");
    while (1) {
        CEXCEPTION_T exception;
        Try {
            sht3xInit(sensor);
            PRINT("Initialise SHT3X");
            break;
        }
        Catch(exception) {
            PRINT("Initialise SHT3X failed; sht3x_ERROR: %02X", exception);
            sleep_ms(500);
        }
    }
}
static _Noreturn void runTest(void) {
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
}
int main(void) {
    initHardware();
    runTest();
}
