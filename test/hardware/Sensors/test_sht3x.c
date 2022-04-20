//
// Created by David P. Federl
//

#include "sht3x_public.h"
#include <pico/time.h>
#include <pico/stdio.h>
#include <hardware/i2c.h>
#include <stdio.h>


int main ( void )
  {
    stdio_init_all ( );
    sleep_ms ( 5000 );
    
    float    temperature, humidity;
    uint32_t serialNumber;
    
    // initialize SHT3X sensor
    sht3x_errorCode sht_errorCode = SHT3X_NO_ERROR;
    while ( 1 )
      {
        sht_errorCode = sht3x_init ( i2c0 );
        if ( sht_errorCode == SHT3X_NO_ERROR )
          {
            break;
          }
        printf ( "INITsht3x_ERROR: %i\r\n", sht_errorCode );
        sleep_ms ( 500 );
      }

#pragma clang diagnostic push
#pragma ide diagnostic   ignored "EndlessLoop"
    while ( 1 )
      {
        sht_errorCode = sht3x_readSerialNumber ( & serialNumber );
        if ( sht_errorCode == SHT3X_NO_ERROR )
          {
            printf ( "SerialNumber: %lu\r\n", serialNumber );
          }
        else
          {
            printf ( "ErrorCode: %i\r\n", sht_errorCode );
          }
        sleep_ms ( 500 );
        
        sht_errorCode = sht3x_getTemperatureAndHumidity ( & temperature, & humidity );
        if ( sht_errorCode == SHT3X_NO_ERROR )
          {
            printf ( "Temperature: %4.2f°C\tHumidity: %4.2f%%RH\r\n", temperature, humidity );
          }
        else
          {
            printf ( "ErrorCode: %i\r\n", sht_errorCode );
          }
        sleep_ms ( 500 );
        
        sleep_ms ( 2000 );
        
        printf ( "\r\n" );
      }
#pragma clang diagnostic pop
    
    return 0;
  }
