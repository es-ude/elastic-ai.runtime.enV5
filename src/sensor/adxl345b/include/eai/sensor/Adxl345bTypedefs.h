#ifndef ENV5_ADXL345B_TYPEDEFS
#define ENV5_ADXL345B_TYPEDEFS

#include <stdbool.h>
#include <stdint.h>

typedef enum adxl345bErrorCode {
    ADXL345B_NO_ERROR = 0x00,
    ADXL345B_SEND_COMMAND_ERROR = 0x01,
    ADXL345B_RECEIVE_DATA_ERROR = 0x02,
    ADXL345B_CHECKSUM_ERROR = 0x03,
    ADXL345B_INIT_ERROR = 0x10,
    ADXL345B_PARM_ERROR = 0x11,
    ADXL345B_CONFIGURATION_ERROR = 0x12,
    ADXL345B_RANGE_ERROR = 0x13,
    ADXL345B_UNDEFINED_ERROR = 0x20,
    ADXL345B_BUFFER_OVERFLOW_ERROR = 0x21,
    ADXL345B_INTERRUP_NOT_SET = 0x22,
    ADXL345B_SELF_TEST_FAILED = 0x31,
    ADXL345B_CALIBRATION_ERROR = 0x32,
} adxl345bErrorCode_t;

/* abstraction to avoid DIRECT dependencies to pico-sdk libraries */
typedef struct i2c_inst i2c_inst_t;

/*! Possible i2c addresses of the sensor.
 *
 * Address should be chosen depending on how the state of the ALT_ADDRESS pin:
 * HIGH/3V3 -> ADXL345B_I2C_ADDRESS
 * LOW/GND  -> ADXL345B_I2C_ALTERNATE_ADDRESS
 */
typedef enum adxl345bI2cAddress {
    ADXL345B_I2C_ADDRESS = 0x1D,
    ADXL345B_I2C_ALTERNATE_ADDRESS = 0x53,
} adxl345bI2cAddress_t;

typedef enum adxl345bRange {
    ADXL345B_2G_RANGE = 0b00000000,
    ADXL345B_4G_RANGE = 0b00000001,
    ADXL345B_8G_RANGE = 0b00000010,
    ADXL345B_16G_RANGE = 0b00000011,
} adxl345bRange_t;

typedef struct adxl345bSensorConfiguration {
    i2c_inst_t *i2c_host;
    adxl345bI2cAddress_t i2c_slave_address;
    bool full_res_mode;
    adxl345bRange_t measurement_range;
} adxl345bSensorConfiguration_t;

/*! ADXL345b Register addresses */
typedef enum adxl345bRegister {
    ADXL345B_REGISTER_DEVID = 0x00,   //!< Device ID (READ ONLY)
    ADXL345B_REGISTER_BW_RATE = 0x2C, //!< enable/disable low power mode and set output data rate
    ADXL345B_REGISTER_POWER_CTL =
        0x2D, //!< enable/disable measurement mode, sleep mode and auto-sleep
    ADXL345B_REGISTER_INT_ENABLE = 0x2E,  //!< enable/disable interrupt sources
    ADXL345B_REGISTER_INT_MAP = 0x2F,     //!< map interrupt sources to INT1 (0) or INT2 (1) pin
    ADXL345B_REGISTER_INT_SOURCE = 0x30,  //!< source of interrupts; clear on read (READ ONLY)
    ADXL345B_REGISTER_DATA_FORMAT = 0x31, //!< set resolution, range and justify mode

    // Measurements
    ADXL345B_REGISTER_OFSX = 0x1E,   //!< X-axis offset
    ADXL345B_REGISTER_OFSY = 0x1F,   //!< Y-axis offset
    ADXL345B_REGISTER_OFSZ = 0x20,   //!< Z-axis offset
    ADXL345B_REGISTER_DATA_X = 0x32, //!< (READ ONLY)
    ADXL345B_REGISTER_DATA_Y = 0x34, //!< (READ ONLY)
    ADXL345B_REGISTER_DATA_Z = 0x36, //!< (READ ONLY)
    ADXL345B_FIFO_CTL = 0x38,        //!< FIFO control
    ADXL345B_FIFO_STATUS = 0x39,     //!< FIFO status (READ ONLY)

    // Tap Detection
    ADXL345B_REGISTER_THRESH_TAP = 0x1D, //!< threshold for tap detection
    ADXL345B_REGISTER_DUR = 0x21,        //!< minimal event length for tap detection
    ADXL345B_REGISTER_LATENT = 0x22,     //!< offset time between taps for double tap
    ADXL345B_REGISTER_WINDOW =
        0x23, //!< time after the first tap during which a second tap can be detected
    ADXL345B_REGISTER_TAP_AXES = 0x2A,       //!< disable/enable axes for tap detection
    ADXL345B_REGISTER_ACT_TAP_STATUS = 0x2B, //!< axis mitigating a tap/activity event (READ ONLY)

    // Activity/Inactivity Detection
    ADXl345B_REGISTER_THRESH_ACT = 0x24,   //!< threshold for activity detection
    ADXL345B_REGISTER_THRESH_INACT = 0x25, //!< threshold for inactivity detection
    ADXL345B_REGISTER_TIME_INACT = 0x26,   //!< time of inactivity until inactivity is triggered
    ADXL345B_REGISTER_ACT_INACT_CTL =
        0x27, //!< axis enable control for activity and inactivity detection

    // Free Fall Detection
    ADXL345B_REGISTER_THRESH_FF = 0x28, //!< Threshold for free-fall detection
    ADXL345B_REGISTER_TIME_FF = 0x29,   //!< minimum event length for free-fall detection
} adxl345bRegister_t;

typedef uint8_t adxl345bConfig_t;

/*! Available bitmasks for output data rates in Hertz.
 *
 * Used for BW_RATE register (0x2C).
 * LPM indicates Low Power Mode.
 */
typedef enum adxl345bOdrFrequency {
    // Low Power Mode Output Data Rates
    ADXL345B_BW_RATE_LPM_12_point_5 = 0b00010111, //!<  represents low power 12.5Hz measurement rate
    ADXL345B_BW_RATE_LPM_25 = 0b00011000,         //!<  represents low power 25Hz measurement rate
    ADXL345B_BW_RATE_LPM_50 = 0b00011001,         //!<  represents low power 50Hz measurement rate
    ADXL345B_BW_RATE_LPM_100 = 0b00011010,        //!<  represents low power 100Hz measurement rate
    ADXL345B_BW_RATE_LPM_200 = 0b00011011,        //!<  represents low power 200Hz measurement rate
    ADXL345B_BW_RATE_LPM_400 = 0b00011100,        //!<  represents low power 400Hz measurement rate

    // Normal Mode Output Data Rates
    ADXL345B_BW_RATE_0_point_1 = 0b00000000,  //!<  represents 0.1Hz measurement rate
    ADXL345B_BW_RATE_0_point_2 = 0b00000001,  //!<  represents 0.2Hz measurement rate
    ADXL345B_BW_RATE_0_point_39 = 0b00000010, //!<<  represents 0.39Hz measurement rate
    ADXL345B_BW_RATE_0_point_78 = 0b00000011, //!<  represents 0.78Hz measurement rate
    ADXL345B_BW_RATE_1_point_56 = 0b00000100, //!<  represents 1.56Hz measurement rate
    ADXL345B_BW_RATE_3_point_13 = 0b00000101, //!<  represents 3.13Hz measurement rate
    ADXL345B_BW_RATE_6_point_25 = 0b00000110, //!<  represents 6.25Hz measurement rate
    ADXL345B_BW_RATE_12_point_5 = 0b00000111, //!<  represents 12.5Hz measurement rate
    ADXL345B_BW_RATE_25 = 0b00001000,         //!<  represents 25Hz measurement rate
    ADXL345B_BW_RATE_50 = 0b00001001,         //!<  represents 50Hz measurement rate
    ADXL345B_BW_RATE_100 = 0b00001010,        //!<  represents 100Hz measurement rate
    ADXL345B_BW_RATE_200 = 0b00001011,        //!<  represents 200Hz measurement rate
    ADXL345B_BW_RATE_400 = 0b00001100,        //!<  represents 400Hz measurement rate
    ADXL345B_BW_RATE_800 = 0b00001101,        //!<  represents 800Hz measurement rate
    ADXL345B_BW_RATE_1600 =
        0b00001110, //!< represents 1600Hz measurement rate (ONLY available with SPI)
    ADXL345B_BW_RATE_3200 =
        0b00001111, //!< represents 3200Hz measurement rate (ONLY available with SPI)
} adxl345bOdrFrequency_t;

/*! Available bitmasks for FIFO modes.
 *
 * Used for FIFO_CTL register (0x38).
 */
typedef enum adxl345bFifoMode {
    ADXL345B_FIFOMODE_BYPASS = 0b00000000,
    ADXL345B_FIFOMODE_FIFO = 0b01000000,
    ADXL345B_FIFOMODE_STREAM = 0b10000000,
    ADXL345B_FIFOMODE_TRIGGER = 0b11000000,
} adxl345bFifoMode_t;

/*! Bitmasks for INT_SOURCE register (0x30) */
typedef enum adxl345bInterrupt {
    ADXL345B_INTERRUPT_DATA_READY = 0b10000000,
    ADXL345B_INTERRUPT_SINGLE_TAP = 0b01000000,
    ADXL345B_INTERRUPT_DOUBLE_TAP = 0b00100000,
    ADXL345B_INTERRUPT_ACTIVITY = 0b00010000,
    ADXL345B_INTERRUPT_INACTIVITY = 0b00001000,
    ADXL345B_INTERRUPT_FREE_FALL = 0b00000100,
    ADXL345B_INTERRUPT_WATERMARK = 0b00000010,
    ADXL345B_INTERRUPT_OVERRUN = 0b00000001,
} adxl345bInterrupt_t;

/*! Available settings for the data format
 *
 * Used for DATA_FORMAT register (0x31).
 */
typedef enum adxl345bDataFormat {
    ADXL345B_DATA_FORMAT_SELF_TEST = 0b10000000, //!< Bit to enable self-test force
    ADXL345B_DATA_FORMAT_FULL_RES = 0b00001000,  //!< Bit to enable full resolution mode
    ADXL345B_DATA_FORMAT_JUSTIFY = 0b00000100,   //!< Bit to enable left justified mode (MSB)
    ADXL345B_DATA_FORMAT_RANGE_2G = 0b00000000,  //!< +/- 2g range
    ADXL345B_DATA_FORMAT_RANGE_4G = 0b00000001,  //!< +/- 4g range
    ADXL345B_DATA_FORMAT_RANGE_8G = 0b00000010,  //!< +/- 8g range
    ADXL345B_DATA_FORMAT_RANGE_16G = 0b00000011, //!< +/- 16g range
} adxl345bDataFormat_t;

/*! Bitmask for different registers. */
typedef enum adxl345bBitmask {
    ADXL345B_POWER_CTL_MEASURE = 0b00001000, //!< enable/disable measurement mode
} adxl345bBitmask_t;

typedef struct adxl345bRawData {
    adxl345bRange_t measurement_range;
    bool full_res_mode;
    uint8_t rawData[6];
} adxl345bRawData_t;

typedef struct adxl345bLsbSample {
    adxl345bRange_t measurement_range;
    bool full_res_mode;
    int16_t x;
    int16_t y;
    int16_t z;
} adxl345bLsbSample_t;

typedef struct adxl345bGValueSample {
    float x;
    float y;
    float z;
} adxl345bGValueSample_t;

#endif /* ENV5_ADXL345B_TYPEDEFS */
