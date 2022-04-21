//
// Created by David P. Federl
//

#include "sht3x_public.h"
#include <pico/bootrom.h>
#include <pico/time.h>
#include <pico/stdio.h>
#include <hardware/i2c.h>
#include <stdio.h>


static void getTemperatureAndHumidity ( )
  {
    float temperature, humidity;
    
    sht3x_errorCode sht_errorCode = sht3x_getTemperatureAndHumidity ( & temperature, & humidity );
    if ( sht_errorCode == SHT3X_NO_ERROR )
      {
        printf ( "Temperature: %4.2f°C\tHumidity: %4.2f%%RH\n", temperature, humidity );
      }
    else
      {
        printf ( "ErrorCode: %i\n", sht_errorCode );
      }
  }

static void getTemperature ( )
  {
    float temperature;
    
    sht3x_errorCode sht_errorCode = sht3x_getTemperature ( & temperature );
    if ( sht_errorCode == SHT3X_NO_ERROR )
      {
        printf ( "Temperature: %4.2f°C\n", temperature );
      }
    else
      {
        printf ( "ErrorCode: %i\n", sht_errorCode );
      }
  }

static void getHumidity ( )
  {
    float humidity;
    
    sht3x_errorCode sht_errorCode = sht3x_getHumidity ( & humidity );
    if ( sht_errorCode == SHT3X_NO_ERROR )
      {
        printf ( "Humidity: %4.2f%%RH\n", humidity );
      }
    else
      {
        printf ( "ErrorCode: %i\n", sht_errorCode );
      }
  }

static void getSerialNumber ( )
  {
    uint32_t serialNumber;
    
    sht3x_errorCode sht_errorCode = sht3x_readSerialNumber ( & serialNumber );
    if ( sht_errorCode == SHT3X_NO_ERROR )
      {
        printf ( "Serial number: %08lx\n", serialNumber );
      }
    else
      {
        printf ( "ErrorCode: %i\r\n", sht_errorCode );
      }
  }

static void enterBootMode ( )
  {
    reset_usb_boot ( 0, 0 );
  }

int main ( void )
  {
    /* enable print to console output */
    stdio_init_all ( );
    /* wait to initialize screen session by user */
    sleep_ms ( 5000 );
    
    /* initialize SHT3X sensor */
    sht3x_errorCode sht_errorCode;
    while ( 1 )
      {
        sht_errorCode = sht3x_init ( i2c0 );
        if ( sht_errorCode == SHT3X_NO_ERROR )
          {
            printf ( "Initialise SHT3X\n" );
            break;
          }
        printf ( "Initialise SHT3X failed; sht3x_ERROR: %02x\n", sht_errorCode );
        sleep_ms ( 500 );
      }
    
    /* test functions of sht3x */
    printf ( "Please enter a (Temp&Humi), t (Temp), h (Humi), s (serialNo), b (Boot mode) to perform an action\n" );
    while ( 1 )
      {
        char input = getchar_timeout_us ( 10000000 );
        
        switch ( input )
          {
            case 'a':
              getTemperatureAndHumidity ( );
            break;
            case 't':
              getTemperature ( );
            break;
            case 'h':
              getHumidity ( );
            break;
            case 's':
              getSerialNumber ( );
            break;
            case 'b':
              enterBootMode ( );
            break;
            default:
              printf ( "Please enter a (Temp&Humi), t (Temp), h (Humi), s (serialNo), b (Boot mode) to perform an action\n" );
            break;
          }
      }
    
    return 0;
  }
