#define SOURCE_FILE "ADXL345-StressTest"
#include "Common.h"
#include "Adxl345b.h"
#include "I2c.h"
#include "FreeRtosTaskWrapper.h"

#include "HardwareTestHelper.h"
#include "EnV5HwController.h"
#include "Esp.h"

#include "Protocol.h"
#include "pico/stdio_usb.h"
#include "pico/bootrom.h"
#include "hardware/i2c.h"


/* endregion HELPER*/

/* region I2C DEFINITION */
i2cConfiguration_t i2cConfig = {
    .i2cInstance = i2c1,
    .frequency = 400000,
    .sdaPin = 6,
    .sclPin = 7,
};
/* endregion I2C DEFINITION */

/* region SENSOR DEFINITION */
adxl345bSensorConfiguration_t sensor = {
    .i2c_slave_address = ADXL345B_I2C_ALTERNATE_ADDRESS,
    .i2c_host = i2c1,
};
/* endregion SENSOR DEFINITION */

#define G_VALUE_BATCH_SECONDS 3

int fibonacci(int n);
int factorial(int n);
void t_test_only_adxl();
void t_stressTest_only_adxlWithFreeRTOS();
static int veryHighFibonacci = 14930352;
void init_adxl();

static void enterBootMode();
float floatToAbs(float input);
_Bool compareFloatsWithinRange(float expected, float actual, float epsilon);

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

void init_adxl() {
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

    /* initialize ADXL345B sensor */
    PRINT("===== START ADXL345B INIT =====");
    adxl345bErrorCode_t errorCode;
    while (1) {
        // adxlInit aktuell standard maessig auf LOW POWER
        errorCode = adxl345bInit(sensor);

        if (errorCode != ADXL345B_NO_ERROR) {
            PRINT("Initialise ADXL345B failed; adxl345b_ERROR: %02X", errorCode);
            sleep_ms(500);
            continue;
        }
        PRINT("Initialised ADXL345B.");

        errorCode = adxl345bWriteConfigurationToSensor(sensor, ADXL345B_REGISTER_BW_RATE,
                                                       ADXL345B_BW_RATE_800);
        if (errorCode != ADXL345B_NO_ERROR) {
            PRINT("Writing Configuration to ADXL345B failed; adxl345b_ERROR: %02X", errorCode);
            sleep_ms(500);
            continue;
        }
        PRINT("Wrote Configuration to ADXL345B.");

        errorCode = adxl345bChangeMeasurementRange(sensor, ADXL345B_16G_RANGE);
        if (errorCode != ADXL345B_NO_ERROR) {
            PRINT("Changing Measurement Range of ADXL345B failed; adxl345b_ERROR: %02X", errorCode);
            sleep_ms(500);
            continue;
        }
        PRINT("Changed Measurement Range of ADXL345B.");
        break;
    }
}

int fibonacci(int n) {
    if (n <= 1) {
        return n;
    } else {
        return fibonacci(n - 1) + fibonacci(n - 2);
    }
}

int factorial(int n) {
    if (n == 0) {
        return 1;
    } else {
        return n * factorial(n - 1);
    }
}

_Noreturn void incrementingTask(void);
_Noreturn void factorialTask(void);
_Noreturn void fibonacciTask(void);
_Noreturn void adxlTask(void);

_Noreturn void incrementingTask(void) {
    uint8_t counter = 0;
    while (true) {
        counter++;
        freeRtosTaskWrapperTaskSleep(10);
    }
};

_Noreturn void factorialTask(void) {
    PRINT("START MEDIOCRE TASK");
    int result;
    int counterN = 1;
    while (true) {
        result = factorial(counterN);
        if (counterN % 5 != 0) {
            counterN++;
        } else {
            freeRtosTaskWrapperTaskSleep(10);
            counterN++;
            PRINT("Factorial: %d\n", result);
            if (counterN >= 100) {
                counterN = 0;
            }
        }
    }
};

_Noreturn void fibonacciTask(void) {
    int result;
    int counterN = 1;
    while (true) {
        result = fibonacci(counterN);
        PRINT("fibonacci: %d\n", result);
        if (result >= veryHighFibonacci) {
            counterN = 0;
        }
        if (counterN % 10 != 0) {
            counterN++;
        } else {
            freeRtosTaskWrapperTaskSleep(10);
            counterN++;
        }
    }
};

void publishTestData() {
    protocolPublishData("testPub", "testData00");
}

void _Noreturn mqttTask(void) {
    PRINT("=== STARTING TEST ===");

    //connectToNetwork();
    // connectToMQTT();

    uint64_t messageCounter = 0;
    while (1) {
        publishTestData();
        messageCounter++;
        freeRtosTaskWrapperTaskSleep(1000);
    }
}

_Noreturn void adxlTask(void) {
    // X sekunden messen
    // warum brauche ich hier eine 0 mehr als bei uns im BallchallengeRepo?
    uint32_t interval = G_VALUE_BATCH_SECONDS * 10000000;
    PRINT("STARTING...");

    while (1) {

        int count = 0;
        uint32_t startTime = time_us_64();
        while (startTime + interval >= time_us_64()) {
            float xAxis, yAxis, zAxis;
            uint8_t rawData[6];
            adxl345bErrorCode_t errorCode = adxl345bGetSingleMeasurement(sensor, rawData);
            if (errorCode == ADXL345B_NO_ERROR) {
                errorCode = adxl345bConvertDataXYZ(&xAxis, &yAxis, &zAxis, rawData);
                if (errorCode == ADXL345B_NO_ERROR) {
                    /* 0.2G equals a deviation of about 1% from the ideal value
                     * this deviation is given by the datasheet as the accepted tolerance
                     * for each axis therefore should epsilon be 0.6G
                     */
                    float sumOfAxis = floatToAbs(xAxis) + floatToAbs(yAxis) + floatToAbs(zAxis);

                    PRINT_DEBUG("  Expected: 01.0000G, Actual: %2.4fG = |%2.4fG| + |%2.4fG| + "
                                "|%2.4fG| = X + Y + Z",
                                sumOfAxis, xAxis, yAxis, zAxis);
                    PRINT_DEBUG(compareFloatsWithinRange(1.0f, sumOfAxis, 0.6f)
                                    ? "  \033[0;32mPASSED\033[0m"
                                    : "  \033[0;31mFAILED\033[0m");
                    count++;
                } else {
                    PRINT("  \033[0;31mFAILED\033[0m; adxl345b_ERROR: %02X", errorCode);
                    continue;
                }
            }
        }

        PRINT("Got %i samples.", count);
        // 1 sekunde schlafen
        // freeRtosTaskWrapperTaskSleep(1000);
    }
}

static void enterBootMode() {
    reset_usb_boot(0, 0);
}

void t_test_only_adxl() {
    adxlTask();
}

void t_stressTest_only_adxlWithFreeRTOS() {
    freeRtosTaskWrapperRegisterTask(adxlTask, "adxlTask", 1, FREERTOS_CORE_0);
}

void t_stressTest_easy() {
    freeRtosTaskWrapperRegisterTask(adxlTask, "adxlTask", 1, FREERTOS_CORE_0);
    freeRtosTaskWrapperRegisterTask(incrementingTask, "incrementingTask", 1, FREERTOS_CORE_1);
}

void t_stressTest_mediocre() {
    freeRtosTaskWrapperRegisterTask(adxlTask, "adxlTask", 1, FREERTOS_CORE_0);
    freeRtosTaskWrapperRegisterTask(factorialTask, "factorialTask", 1, FREERTOS_CORE_1);
}

void t_stressTest_hard() {
    freeRtosTaskWrapperRegisterTask(adxlTask, "adxlTask", 1, FREERTOS_CORE_0);
    freeRtosTaskWrapperRegisterTask(fibonacciTask, "fibonacciTask", 1, FREERTOS_CORE_1);
}

void t_stressTest_withMQTT() {
    freeRtosTaskWrapperRegisterTask(adxlTask, "adxlTask", 1, FREERTOS_CORE_0);
    freeRtosTaskWrapperRegisterTask(mqttTask, "mqttTask", 1, FREERTOS_CORE_1);
}

_Noreturn void t_stressTest_enterBootModeTask(void) {
    while (true) {
        if (getchar_timeout_us(10) == 'r' || !stdio_usb_connected()) {
            // enter boot mode if 'r' was received
            reset_usb_boot(0, 0);
        }

        // watchdog update needs to be performed frequent, otherwise the device will crash
        // watchdog_update();
        freeRtosTaskWrapperTaskSleep(1000);
    }
}

// TODO: adxl_task mit MQTT_task testen

int main(void) {
    // init stdio and esp
    stdio_init_all();
    PRINT("TEST");
    // Did we crash last time -> reboot into boot rom mode
    /*if (watchdog_enable_caused_reboot()) {
        reset_usb_boot(0, 0);
    }*/

    env5HwControllerInit();



    // das hier passiert vorher noch im Ballchallenge-Repo. Erkenne keinen Zusammenhang.
    //espInit();
    //  initialize WiFi and MQTT broker
    //connectToNetwork();
    //connectToMqttBroker();

    // vor der init schlafen lassen um sicher zu stellen, dass pico nicht zu schnell ist und adxl zu
    // langsam nach reboot
    sleep_ms(1000);
    init_adxl();

    while (1) {
        char input = getchar_timeout_us(10000000); /* 10 seconds wait */

        switch (input) {
        case 'o':
            t_test_only_adxl();
            freeRtosTaskWrapperStartScheduler();
            break;
        case 'f':
            t_stressTest_only_adxlWithFreeRTOS();
            freeRtosTaskWrapperStartScheduler();
            break;
        case 'e':
            t_stressTest_easy();
            freeRtosTaskWrapperStartScheduler();
            break;
        case 'm':
            t_stressTest_mediocre();
            freeRtosTaskWrapperStartScheduler();
            break;
        case 'h':
            t_stressTest_hard();
            freeRtosTaskWrapperStartScheduler();
            break;
        case 'q':
            t_stressTest_withMQTT();
            break;

        case 'b':
            enterBootMode();
            break;
        default:
            PRINT("Please enter to request for intensity:\n o (only adxl),\n f (freeRTOS with "
                  "adxl),\n e (easy),\n m (mediocre),\n h "
                  "(hard),\n q (adxl with mqtt),\n b (Boot mode)");
            break;
        }
    }

    return 0;
}
