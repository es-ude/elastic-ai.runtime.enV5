#define SOURCE_FILE "MAIN"
#include "Adxl345b.h"
#include "Adxl345bTypedefs.h"
#include "Common.h"
#include "EnV5HwController.h"
#include "Esp.h"
#include "FlashLoader.h"
#include "FreeRtosTaskWrapper.h"
#include "I2c.h"
#include "MqttBroker.h"
#include "Network.h"
#include "Ota.h"
#include "hardware/flash.h"
#include "hardware/i2c.h"
#include "hardware/structs/watchdog.h"
#include "pico/stdio_usb.h"
#include "pico/time.h"

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

int main()
{
    stdio_init_all();
    espInit();
    env5HwControllerLedsInit();

    networkTryToConnectToNetworkUntilSuccessful();
    mqttBrokerConnectToBrokerUntilSuccessful("eip://uni-due.de/es", "enV5");

    PRINT("scratch: %lu", watchdog_hw->scratch[0]);
    
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
        errorCode = adxl345bInit(sensor);
        if (errorCode == ADXL345B_NO_ERROR) {
            PRINT("Initialised ADXL345B.");
            break;
        }
        PRINT("Initialise ADXL345B failed; adxl345b_ERROR: %02X", errorCode);
        sleep_ms(500);
    }
    
    if(watchdog_hw->scratch[0] == FLASH_APP_UPDATED)
    {
        PRINT("Application just updated!");
        watchdog_hw->scratch[0] = 0;
    } else {
        PRINT("Application not updated!");
    }

    freeRtosTaskWrapperRegisterTask(flashHex, "receiveData", 5, FREERTOS_CORE_1);

    PRINT("STARTING SCHEDULER...");

    freeRtosTaskWrapperStartScheduler();

    return 0;
}
