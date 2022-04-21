//
// Created by David P. Federl
//

#include "adxl345b_public.h"
#include <pico/bootrom.h>
#include <pico/time.h>
#include <pico/stdio.h>
#include <hardware/i2c.h>
#include <math.h>
#include <stdio.h>


_Bool compareFloatsWithinRange ( float expected, float actual, float epsilon )
  {
    return fabs ( expected - actual ) < epsilon;
  }

static void getSerialNumber ( )
  {
    uint8_t serialNumber = 0;
    
    adxl345b_errorCode errorCode = adxl345b_readSerialNumber ( & serialNumber );
    if ( errorCode == ADXL345B_NO_ERROR )
      {
        printf ( "SerialNumber:\n" );
        printf ( "  Expected: 0xE5, Actual: 0x%02X\n", serialNumber );
        printf ( serialNumber == 0xE5 ? "  \033[0;32mPASSED\033[0m\n" : "  \033[0;31mFAILED\033[0m\n" );
      }
    else
      {
        printf ( "ErrorCode: %i\n", errorCode );
      }
    
  }

static void getGValue ( )
  {
    float xAxis = 0, yAxis = 0, zAxis = 0;
    
    adxl345b_errorCode errorCode = adxl345b_readMeasurements ( & xAxis, & yAxis, & zAxis );
    if ( errorCode == ADXL345B_NO_ERROR )
      {
        printf ( "Measurements:\n" );
        printf ( "  (X,Y,Z) = (%2.4f, %2.4f, %2.4f)\n", xAxis, yAxis, zAxis );
        printf ( compareFloatsWithinRange ( 1.0f, xAxis + yAxis + zAxis, 0.1f ) ? "  \033[0;32mPASSED\033[0m\n" : "  \033[0;31mFAILED\033[0m\n" );
      }
    else
      {
        printf ( "ErrorCode: %i\n", errorCode );
      }
  }

static void enterBootMode ( )
  {
    reset_usb_boot ( 0, 0 );
  }


int main ( void )
  {
    /* enable print to console */
    stdio_init_all ( );
    
    uint8_t serialNumber = 0;
    float   xAxis        = 0, yAxis = 0, zAxis = 0;
    
    /*initialize ADXL345B sensor */
    adxl345b_errorCode errorCode = ADXL345B_NO_ERROR;
    while ( 1 )
      {
        errorCode = adxl345b_init ( i2c0 );
        if ( errorCode == ADXL345B_NO_ERROR )
          {
            printf ( "Initialised ADXL345B.\n" );
            break;
          }
        printf ( "Initialise ADXL345B failed; adxl345b_ERROR: %02x\n", errorCode );
        sleep_ms ( 500 );
      }
    
    /* test function of adxl345b */
    while ( 1 )
      {
        char input = getchar_timeout_us ( 1000 );
        
        switch ( input )
          {
            case 'g':
              getGValue ( );
            break;
            case 's':
              getSerialNumber ( );
            break;
            case 'b':
              enterBootMode ( );
            break;
            default:
              printf ( "Please enter g (G value), s (serialNo), b (Boot mode) to perfom an action" );
            break;
          }
      }
    return 0;
  }


