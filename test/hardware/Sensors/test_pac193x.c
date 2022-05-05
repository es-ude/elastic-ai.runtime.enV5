//
// Created by David P. Federl
//

#include "pac193x.h"
#include <hardware/i2c.h>
#include <math.h>
#include <pico/stdlib.h>
#include <stdio.h>

int main(void) {
    stdio_init_all();
    sleep_ms(5000);
    
    /* initialize sensor */
    pac193x_errorCode errorCode;
    float             resistorValues[4]   = {0.82f, 0.82f, 0, 0};
    uint8_t           highestChannelInUse = 2;
    float             time_past;
    absolute_time_t   startTime, currentTime;
    while (1) {
        errorCode = pac193x_init(i2c1, resistorValues, highestChannelInUse);
        if (errorCode == PAC193X_NO_ERROR) {
            break;
          }
        printf("INIT_ERROR: %i\r\n", errorCode);
        sleep_ms(500);
      }
    
    startTime = get_absolute_time();
#pragma clang diagnostic push
#pragma ide diagnostic   ignored "EndlessLoop"
    while (1) {
        /* get ID of sensor */
        pac193x_info senseIDs;
        errorCode = pac193x_getSensorInfo(&senseIDs);
        if (errorCode != PAC193X_NO_ERROR) {
            printf("PAC193X ErrorCode: %i\r\n", errorCode);
          } else {
            /* expected values for PAC1933:
             *   Product ID: 0x5A
             *   Manufacturer ID: 0x5D
             *   Revision ID: 0x03
             */
            printf("Expected Product ID: 0x%2x; Manufacture ID: 0x%2x; Revision ID: 0x%02x\r\n", 0x5A, 0x5D,
                   0x03);
            printf("Actual Product ID: 0x%2x; Manufacture ID: 0x%2x; Revision ID: 0x%2x\r\n",
                   senseIDs.product_id, senseIDs.manufacturer_id, senseIDs.revision_id);
            bool valid_product_id = (senseIDs.product_id >= 0x59) && (senseIDs.product_id <= 0x5B);
            if (valid_product_id && 0x5D == senseIDs.manufacturer_id && 0x03 == senseIDs.revision_id) {
                printf("Valid product ID.\r\n\r\n");
              } else {
                printf("IDs don't match!\r\n\r\n");
              }
          }
        sleep_ms(500);
        
        /* read *all* values from sensor */
        pac193x_measurements measurements;
        errorCode = pac193x_getAllMeasurementsForChannel(PAC193X_CHANNEL_SENSORS, &measurements);
        if (errorCode != PAC193X_NO_ERROR) {
            printf("PAC193X ErrorCode: %i\r\n", errorCode);
          } else {
            printf("Measurements for channel 1 (Sensors):\r\n  VSource=%4.6fV\r\n  VSense=%4.6fmV\r\n  "
                   "ISense=%4.6fmA\r\n",
                   measurements.voltageSource, measurements.voltageSense * 1000, measurements.iSense * 1000);
            printf("RSense_expected=%4.2fΩ, RSense_actual=%4.2fΩ ", resistorValues[0],
                   measurements.voltageSense / (measurements.iSense));
            if (fabs(resistorValues[0] - measurements.voltageSense / (measurements.iSense)) < 0.01) {
                printf("SUCCESS\r\n");
              } else {
                printf("FAIL\r\n");
              }
            printf("Calculating from VPower:         PActual=%4.6f W\r\n", measurements.powerActual);
            printf("Calculating from Vsource,ISense: PActual=%4.6f W\r\n",
                   measurements.iSense * measurements.voltageSource);
            printf("Checking if both values are approximately the same: ");
            if (fabs(measurements.powerActual - measurements.iSense * measurements.voltageSource) < 1e-5) {
                printf("SUCCESS\r\n");
              } else {
                printf("FAIL\r\n");
              }
            printf("Energy = %4.6f Ws\r\n", measurements.energy);
            time_past = (float)absolute_time_diff_us(startTime, currentTime) * 1e-6;
            printf("Time past : %4.6f s\r\n", time_past);
            printf("Energy / Time  = %4.6f W\r\n", measurements.energy / time_past);
          }
        errorCode   = pac193x_getAllMeasurementsForChannel(PAC193X_CHANNEL_WIFI, &measurements);
        currentTime = get_absolute_time();
        if (errorCode != PAC193X_NO_ERROR) {
            printf("PAC193X ErrorCode: %i\r\n", errorCode);
          } else {
            printf("Measurements for channel 2 (Wifi):\r\n  VSource=%4.6fV\r\n  VSense=%4.6fmV\r\n  "
                   "ISense=%4.6fmA\r\n",
                   measurements.voltageSource, measurements.voltageSense * 1000, measurements.iSense * 1000);
            printf("RSense_expected=%4.2fΩ, RSense_actual=%4.2fΩ ", resistorValues[1],
                   measurements.voltageSense / (measurements.iSense));
            if (fabs(resistorValues[0] - measurements.voltageSense / (measurements.iSense)) < 0.1) {
                printf("SUCCESS\r\n");
              } else {
                printf("FAIL\r\n");
              }
            printf("Calculating from VPower:         PActual=%4.6f W\r\n", measurements.powerActual);
            printf("Calculating from Vsource,Isense: PActual=%4.6f W\r\n",
                   measurements.iSense * measurements.voltageSource);
            printf("Checking if both values are approximately the same: ");
            if (fabs(measurements.powerActual - measurements.iSense * measurements.voltageSource) < 10e-5) {
                printf("SUCCESS\r\n");
              } else {
                printf("FAIL\r\n");
              }
            printf("Energy = %4.6f Ws\r\n", measurements.energy);
            time_past = (float)absolute_time_diff_us(startTime, currentTime) * 0.000001;
            printf("Time past : %4.6f s\r\n", time_past);
            printf("Energy / Time  = %4.6f W\r\n", measurements.energy / time_past);
          }
        sleep_ms(500);
        
        sleep_ms(2000);
        printf("\r\n");
      }
#pragma clang diagnostic pop
    
    return 0;
  }
