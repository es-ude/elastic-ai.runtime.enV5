//
// Created by David P. Federl
//

#ifndef ENV5_ADXL345B_TYPEDEFS
#define ENV5_ADXL345B_TYPEDEFS

#include <stdint.h>


typedef struct i2c_inst i2c_inst_t;

typedef struct
  {
    uint8_t    i2c_slave_address;
    i2c_inst_t * i2c_host;
  }                     adxl345b_i2cSensorConfiguration;

enum
  {
    ADXL345B_REGISTER_DEVICE_ID          = 0x00,
    ADXL345B_TAP_THRESHOLD               = 0x1D,
    ADXL345B_OFFSET_X                    = 0x1E,
    ADXL345B_OFFSET_Y                    = 0x1F,
    ADXL345B_OFFSET_Z                    = 0x20,
    ADXL345B_TAB_DURATION                = 0x21,
    ADXL345B_TAB_LATENCY                 = 0x22,
    ADXL345B_TAB_WINDOW                  = 0x23,
    ADXl345B_ACTIVITY_THRESHOLD          = 0x24,
    ADXL345B_INACTIVITY_THERSHOLD        = 0x25,
    ADXL345B_INACTIVITY_TIME             = 0x26,
    ADXL345B_AXIS_FOR_ACTIVITY_DETECTION = 0x27,
    ADXL345B_FREE_FALL_THERSHOLD         = 0x28,
    ADXL345B_FREE_FALL_TIME              = 0x29,
    ADXL345B_TAP_AXIS                    = 0x2A,
    ADXL345B_TAP_AXIS_SOURCE             = 0x2B,
    ADXL345B_REGISTER_BW_RATE            = 0x2C,
    ADXL345B_REGISTER_POWER_CONTROL      = 0x2D,
    ADXL345B_REGISTER_INTERRUPT_ENABLE   = 0x2E,
    ADXL345B_REGISTER_INTERRUPT_MAP      = 0x2F,
    ADXL345B_REGISTER_INTERRUPT_SOURCE   = 0x30,
    ADXL345B_REGISTER_DATA_FORMAT        = 0x31,
    ADXL345B_REGISTER_DATA_X             = 0x32,
    ADXL345B_REGISTER_DATA_Y             = 0x34,
    ADXL345B_REGISTER_DATA_Z             = 0x36,
    ADXL345B_FIFO_CONTROL                = 0x38,
    ADXL345B_FIFO_STATUS                 = 0x39
  };
typedef uint8_t         adxl345b_register;
typedef uint8_t         adxl345b_command;

enum
  {
    ADXL345B_16G_RANGE_SETTING = 0b00000011,
    ADXL345B_8G_RANGE_SETTING  = 0b00000010,
    ADXL345B_4G_RANGE_SETTING  = 0b00000001,
    ADXL345B_2G_RANGE_SETTING  = 0b00000000,
  };
typedef uint8_t         adxl345b_rangeSettings;
typedef uint8_t         adxl345b_msbMask;
typedef float           adxl345b_scaleFactor;

enum
  {
    ADXL345B_16G_MAX_VALUE = 16,
    ADXL345B_8G_MAX_VALUE  = 8,
    ADXL345B_4G_MAX_VALUE  = 4,
    ADXL345B_2G_MAX_VALUE  = 2,
  };
typedef float           adxl345b_maxValue;

enum
  {
    ADXL345B_NO_ERROR            = 0x00,
    ADXL345B_SEND_COMMAND_ERROR  = 0x01,
    ADXL345B_RECEIVE_DATA_ERROR  = 0x02,
    ADXL345B_CHECKSUM_ERROR      = 0x03,
    ADXL345B_INIT_ERROR          = 0x10,
    ADXL345B_PARM_ERROR          = 0x11,
    ADXL345B_CONFIGURATION_ERROR = 0x12,
    ADXL345B_UNDEFINED_ERROR     = 0x20,
  };
typedef uint8_t         adxl345b_errorCode;

#endif /* ENV5_ADXL345B_TYPEDEFS */
