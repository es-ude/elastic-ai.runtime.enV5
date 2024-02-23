#define SOURCE_FILE "ADXL345-StressTest"

#include "Adxl345b.h"
#include "Common.h"
#include <hardware/i2c.h>
#include <pico/bootrom.h>
#include <pico/stdio_usb.h>
#include <pico/time.h>
#include <string.h>
#include <malloc.h>
#define G_VALUE_BATCH_SECONDS 1

int main(void) {
    /* enable print to console */
    stdio_init_all( );
    // wait for user console to connect
    while ((!stdio_usb_connected( ))) { }
    sleep_ms( 500);
    
    /* initialize ADXL345B sensor */
    PRINT( "START INIT" );
    adxl345bErrorCode_t errorCode;
    while (1) {
       errorCode = adxl345bInit(i2c1, ADXL345B_I2C_ALTERNATE_ADDRESS);
      i2c_set_baudrate(i2c1, 1600000);
      if (errorCode == ADXL345B_NO_ERROR) {
        PRINT( "Initialised ADXL345B." );
        break;
      }
      PRINT( "Initialise ADXL345B failed; adxl345b_ERROR: %02X", errorCode );
      sleep_ms( 500 );
    }
    adxl345bWriteConfigurationToSensor(ADXL345B_REGISTER_BW_RATE, ADXL345B_BW_RATE_3200);
    adxl345bChangeMeasurementRange(ADXL345B_16G_RANGE);
    
    uint32_t interval = G_VALUE_BATCH_SECONDS * 1000000;
    PRINT("STARTING...");
    
    while (true) {
      
      int count = 0;
      
      uint32_t currentTime = time_us_64( );
      uint32_t startTime = time_us_64( );
      while (startTime + interval >= currentTime) {
          currentTime = time_us_64();
          
          float xAxis, yAxis, zAxis;
          adxl345bReadMeasurements(&xAxis, &yAxis, &zAxis);
          
          
          count++;
      }
      
      PRINT("%i", count);
    }
}

  